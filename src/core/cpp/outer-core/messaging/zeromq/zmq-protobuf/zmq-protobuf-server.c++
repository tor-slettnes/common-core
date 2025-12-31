/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-server.c++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - server
/// @author Tor Slettnes
//==============================================================================

#include "zmq-protobuf-server.h++"
#include "protobuf-event-types.h++"
#include "platform/path.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    ProtoBufServer::ProtoBufServer(const std::string &bind_address,
                                   const std::string &channel_name,
                                   RequestHandlerMap &&handler_map,
                                   Role role)
        : Super(bind_address, channel_name, role),
          handler_map(std::move(handler_map))
    {
    }

    void ProtoBufServer::initialize()
    {
        Super::initialize();
        for (const auto &[interface_name, handler] : this->handler_map)
        {
            handler->initialize();
        }
    }

    void ProtoBufServer::deinitialize()
    {
        for (const auto &[interface_name, handler] : this->handler_map)
        {
            handler->deinitialize();
        }
        Super::deinitialize();
    }

    void ProtoBufServer::process_binary_request(const types::ByteVector &packed_request,
                                                types::ByteVector *packed_reply)
    {
        cc::protobuf::request_reply::Request request;
        cc::protobuf::request_reply::Reply reply;

        if (request.ParseFromArray(packed_request.data(), packed_request.size()))
        {
            log_trace("Processing ProtoBuf RPC request: ", request);
            this->process_protobuf_request(request, &reply);
            log_trace("Sending ProtoBuf RPC response: ", reply);
        }
        else
        {
            this->insert_error_response(
                &reply,
                cc::protobuf::request_reply::STATUS_INVALID,
                "Failed to deserialize ProtoBuf request",
                {{"channel", this->channel_name()},
                 {"payload", packed_request}});
        }

        protobuf::to_bytes(reply, packed_reply);
    }

    void ProtoBufServer::process_protobuf_request(const cc::protobuf::request_reply::Request &request,
                                                  cc::protobuf::request_reply::Reply *reply)
    {
        reply->set_client_id(request.client_id());
        reply->set_request_id(request.request_id());

        if (RequestHandlerPtr handler = this->handler_map.get(request.interface_name()))
        {
            return handler->process_method_request(request, reply);
        }
        else
        {
            this->insert_error_response(
                reply,
                cc::protobuf::request_reply::STATUS_INVALID,
                "No such interface",
                {{"channel", this->channel_name()},
                 {"interface", request.interface_name()}});
        }
    }

    void ProtoBufServer::insert_error_response(cc::protobuf::request_reply::Reply *reply,
                                               cc::protobuf::request_reply::StatusCode status_code,
                                               const std::string &text,
                                               const types::KeyValueMap &attributes)
    {
        cc::protobuf::request_reply::Status *status = reply->mutable_status();
        status->set_code(status_code);

        status::Error event(text,                                           // text
                            core::status::Domain::APPLICATION,              // domain
                            platform::path->exec_name(),                    // origin
                            static_cast<status::Error::Code>(status_code),  // code
                            cc::protobuf::request_reply::StatusCode_Name(status_code),           // symbol
                            core::status::Level::ERROR,                    // level
                            {},                                             // timepoint
                            attributes);                                    // attributes

        ::protobuf::encode(event, status->mutable_details());
    }

}  // namespace core::zmq
