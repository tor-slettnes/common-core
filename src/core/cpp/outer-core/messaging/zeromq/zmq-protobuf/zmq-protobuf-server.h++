/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-server.h++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-requesthandler.h++"
#include "zmq-responder.h++"
#include "protobuf-message.h++"
#include "types/valuemap.h++"

#include "cc/protobuf/request_reply/request_reply.pb.h"

namespace core::zmq
{
    class ProtoBufServer : public Responder
    {
        using This = ProtoBufServer;
        using Super = Responder;

    protected:
        using RequestHandlerPtr = std::shared_ptr<ProtoBufRequestHandler>;
        using RequestHandlerMap = core::types::ValueMap<std::string, RequestHandlerPtr>;

    protected:
        ProtoBufServer(const std::string &bind_address,
                       const std::string &channel_name,
                       RequestHandlerMap &&handler_map = {},
                       Role role = Role::HOST);

    public:
        void initialize() override;
        void deinitialize() override;

    protected:
        void process_binary_request(const types::ByteVector &packed_request,
                                    types::ByteVector *packed_reply) override;

        void process_protobuf_request(const cc::protobuf::request_reply::Request &request,
                                      cc::protobuf::request_reply::Reply *reply);

    private:
        void insert_error_response(cc::protobuf::request_reply::Reply *reply,
                                   cc::protobuf::request_reply::StatusCode status_code,
                                   const std::string &text,
                                   const types::KeyValueMap &attributes);

    private:
        RequestHandlerMap handler_map;
    };
}  // namespace core::zmq
