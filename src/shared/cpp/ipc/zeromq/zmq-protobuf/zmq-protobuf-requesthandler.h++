/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-requesthandler.h++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - request handler
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-responder.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-message.h++"
#include "types/valuemap.h++"

#include "request_reply.pb.h"

namespace cc::zmq
{
    class ProtoBufRequestHandler
    {
        using This = ProtoBufRequestHandler;

    protected:
        ProtoBufRequestHandler(const std::string &interface_name);

    public:
        std::string interface_name() const;
        virtual void initialize() {}
        virtual void deinitialize();

    public:
        void process_method_request(const CC::RR::Request &request,
                                    CC::RR::Reply *reply);

    protected:
        template <class ReplyType, class RequestType, class Subclass>
        void add_handler(const std::string &method_name,
                         ReplyType (Subclass::*method)(const RequestType &))
        {
            this->handler_map.insert_or_assign(
                method_name,
                [=](const CC::RR::Parameter &req_param, CC::RR::Parameter *rep_param) {
                    RequestType req;
                    req.ParseFromString(req_param.serialized_proto());

                    ReplyType rep = (static_cast<Subclass *>(this)->*method)(req);
                    rep.SerializeToString(rep_param->mutable_serialized_proto());
                });
        }

        void clear_handlers();

    private:
        std::string full_method_name(const std::string &basename) const;

    private:
        std::string interface_name_;

        using MethodHandler =
            std::function<void(const CC::RR::Parameter &request,
                               CC::RR::Parameter *reply)>;
        using MethodHandlerMap =
            types::ValueMap<std::string, MethodHandler>;

        MethodHandlerMap handler_map;
    };
}  // namespace cc::zmq
