/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-server.c++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - server
/// @author Tor Slettnes <tor@slett.net>
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
        cc::rr::Request request;
        cc::rr::Reply reply;

        logf_debug("Received binary request: %s", packed_request);

        if (request.ParseFromArray(packed_request.data(), packed_request.size()))
        {
            this->process_protobuf_request(request, &reply);
        }
        else
        {
            this->insert_error_response(
                &reply,
                cc::rr::STATUS_INVALID,
                "Failed to deserialize ProtoBuf request",
                {{"channel", this->channel_name()},
                 {"payload", packed_request}});
        }

        protobuf::to_bytes(reply, packed_reply);
    }

    void ProtoBufServer::process_protobuf_request(const cc::rr::Request &request,
                                                  cc::rr::Reply *reply)
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
                cc::rr::STATUS_INVALID,
                "No such interface",
                {{"channel", this->channel_name()},
                 {"interface", request.interface_name()}});
        }
    }

    void ProtoBufServer::insert_error_response(cc::rr::Reply *reply,
                                               cc::rr::StatusCode status_code,
                                               const std::string &text,
                                               const types::KeyValueMap &attributes)
    {
        cc::rr::Status *status = reply->mutable_status();
        status->set_code(status_code);

        status::Error event(text,                                           // text
                            core::status::Domain::APPLICATION,              // domain
                            platform::path->exec_name(),                    // origin
                            static_cast<status::Error::Code>(status_code),  // code
                            cc::rr::StatusCode_Name(status_code),           // symbol
                            core::status::Level::ERROR,                    // level
                            {},                                             // timepoint
                            attributes);                                    // attributes

        ::protobuf::encode(event, status->mutable_details());
    }

}  // namespace core::zmq
