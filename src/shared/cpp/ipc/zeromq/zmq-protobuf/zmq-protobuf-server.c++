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

namespace cc::zmq
{
    ProtoBufServer::ProtoBufServer(const std::string &bind_address,
                                   const std::string &channel_name,
                                   RequestHandlerMap &&handler_map)
        : Super(bind_address, channel_name),
          handler_map(std::move(handler_map))
    {
    }

    void ProtoBufServer::initialize()
    {
        Super::initialize();
        for (const auto &[interface_name, handler]: this->handler_map)
        {
            handler->initialize();
        }
    }

    void ProtoBufServer::deinitialize()
    {
        for (const auto &[interface_name, handler]: this->handler_map)
        {
            handler->deinitialize();
        }
        Super::deinitialize();
    }

    void ProtoBufServer::process_binary_request(const types::ByteVector &packed_request,
                                                types::ByteVector *packed_reply)
    {
        CC::RR::Request request;
        CC::RR::Reply reply;

        logf_debug("Received binary request: %s", packed_request);

        if (request.ParseFromArray(packed_request.data(), packed_request.size()))
        {
            this->process_protobuf_request(request, &reply);
        }
        else
        {
            this->insert_error_response(
                &reply,
                CC::RR::STATUS_INVALID,
                "Failed to deserialize ProtoBuf request",
                cc::status::Flow::CANCELLED,
                {{"channel", this->channel_name()},
                 {"payload", packed_request.to_hex(true, 4)}});
        }

        protobuf::to_bytes(reply, packed_reply);
    }

    void ProtoBufServer::process_protobuf_request(const CC::RR::Request &request,
                                                   CC::RR::Reply *reply)
    {
        if (RequestHandlerPtr handler = this->handler_map.get(request.interface_name()))
        {
            return handler->process_method_request(request, reply);
        }
        else
        {
            return this->insert_error_response(
                reply,
                CC::RR::STATUS_INVALID,
                "No such interface",
                cc::status::Flow::CANCELLED,
                {{"channel", this->channel_name()},
                 {"interface", request.interface_name()}});
        }
    }

    void ProtoBufServer::insert_error_response(CC::RR::Reply *reply,
                                               CC::RR::StatusCode status_code,
                                               const std::string &text,
                                               cc::status::Flow flow,
                                               const types::KeyValueMap &attributes)
    {
        CC::RR::Status *status = reply->mutable_status();
        status->set_code(status_code);

        status::Event event(text,
                            cc::status::Domain::APPLICATION,
                            platform::path->exec_name(),
                            static_cast<status::Event::Code>(status_code),
                            CC::RR::StatusCode_Name(status_code),
                            cc::status::Level::FAILED,
                            flow,
                            {},
                            attributes);

        protobuf::encode(event, status->mutable_details());
    }

}  // namespace cc::zmq
