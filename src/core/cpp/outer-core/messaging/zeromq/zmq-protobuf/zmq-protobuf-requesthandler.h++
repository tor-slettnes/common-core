/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-requesthandler.h++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - request handler
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "protobuf-standard-types.h++"
#include "protobuf-message.h++"
#include "types/valuemap.h++"

#include "cc/protobuf/request_reply/request_reply.pb.h"

namespace core::zmq
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
        void process_method_request(const cc::protobuf::request_reply::Request &request,
                                    cc::protobuf::request_reply::Reply *reply);

    protected:
        template <class ReplyType, class RequestType, class Subclass>
        void add_handler(const std::string &method_name,
                         void (Subclass::*method)(const RequestType &, ReplyType *))
        {
            this->handler_map.insert_or_assign(
                method_name,
                [=](const cc::protobuf::request_reply::Parameter &req_param, cc::protobuf::request_reply::Parameter *rep_param) {
                    RequestType req;
                    req.ParseFromString(req_param.serialized_proto());

                    ReplyType rep;
                    (static_cast<Subclass *>(this)->*method)(req, &rep);
                    rep.SerializeToString(rep_param->mutable_serialized_proto());
                });
        }

        void clear_handlers();

    private:
        std::string full_method_name(const std::string &basename) const;

    private:
        std::string interface_name_;

        using MethodHandler =
            std::function<void(const cc::protobuf::request_reply::Parameter &request,
                               cc::protobuf::request_reply::Parameter *reply)>;
        using MethodHandlerMap =
            types::ValueMap<std::string, MethodHandler>;

        MethodHandlerMap handler_map;
    };
}  // namespace core::zmq
