/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-client.h++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-requester.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-message.h++"

#include "request_reply.pb.h"

namespace cc::zmq
{
    class ProtoBufClient : public Requester
    {
        using This = ProtoBufClient;
        using Super = Requester;

    protected:
        ProtoBufClient(const std::string &host_address,
                       const std::string &class_name,
                       const std::string &channel_name,
                       const std::string &interface_name);

    public:
        std::string interface_name() const;

    protected:
        //======================================================================
        // Methods to send/receive populated ProboBuf Request/Reply structures

        void send_request(const CC::RR::Request &request,
                          ::zmq::send_flags flags = ::zmq::send_flags::none);

        bool receive_reply(CC::RR::Reply *reply,
                           ::zmq::recv_flags flags = ::zmq::recv_flags::none);

        bool send_receive(const CC::RR::Request &request,
                          CC::RR::Reply *reply,
                          ::zmq::send_flags send_flags = ::zmq::send_flags::none,
                          ::zmq::recv_flags recv_flags = ::zmq::recv_flags::none);

        //======================================================================
        // Invoke method with populated Input/Output parameter messages

        void send_invocation(const std::string &method_name,
                             const CC::RR::Parameter &param,
                             ::zmq::send_flags send_flags = ::zmq::send_flags::none);

        bool read_result(CC::RR::Parameter *param,
                         CC::RR::Status *status,
                         ::zmq::recv_flags flags = ::zmq::recv_flags::none);

        bool read_result(CC::RR::Parameter *param,
                         ::zmq::recv_flags recv_flags = ::zmq::recv_flags::none);

        //======================================================================
        // Invoke method  with variant request/reply parameters

        types::Value call(const std::string &method_name,
                          const types::Value &request,
                          ::zmq::send_flags send_flags = ::zmq::send_flags::none,
                          ::zmq::recv_flags recv_flags = ::zmq::recv_flags::none);

        //======================================================================
        // Invoke method with ProtoBuf request/reply parameters

    private:
        void send_protobuf_invocation(const std::string method_name,
                                      const google::protobuf::Message &request,
                                      ::zmq::send_flags send_flags);

        bool read_protobuf_result(types::ByteArray *bytes,
                                  ::zmq::recv_flags recv_flags);

    public:
        template <class ResponseType = google::protobuf::Empty>
        ResponseType call(
            const std::string method_name,
            const google::protobuf::Message &request = google::protobuf::Empty(),
            ::zmq::send_flags send_flags = ::zmq::send_flags::none,
            ::zmq::recv_flags recv_flags = ::zmq::recv_flags::none)
        {
            this->send_protobuf_invocation(method_name, request, send_flags);

            types::ByteArray bytes;
            if (this->read_protobuf_result(&bytes, recv_flags))
            {
                return protobuf::to_message<ResponseType>(bytes);
            }
            else
            {
                return {};
            }
        }

    private:
        static uint last_client_id;
        std::string interface_name_;
        uint client_id;
        uint last_request_id;
    };
};  // namespace cc::zmq
