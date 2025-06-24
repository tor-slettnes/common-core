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
#include "logging/logging.h++"

#include "request_reply.pb.h"

namespace core::zmq
{
    class ProtoBufClient : public Requester
    {
        using This = ProtoBufClient;
        using Super = Requester;

    protected:
        ProtoBufClient(const std::string &address,
                       const std::string &channel_name,
                       const std::string &interface_name,
                       Role role = Role::SATELLITE);

    public:
        std::string interface_name() const;

    protected:
        //======================================================================
        // Methods to send/receive populated ProboBuf Request/Reply structures

        void send_request(const cc::rr::Request &request,
                          SendFlags flags = 0) const;

        bool receive_reply(cc::rr::Reply *reply,
                           RecvFlags flags = 0) const;

        bool send_receive(const cc::rr::Request &request,
                          cc::rr::Reply *reply,
                          SendFlags send_flags = 0,
                          RecvFlags recv_flags = 0) const;

    private:
        uint next_request_id() const;

        //======================================================================
        // Invoke method with populated Input/Output parameter messages

        void send_invocation(const std::string &method_name,
                             const cc::rr::Parameter &param,
                             SendFlags send_flags = 0) const;

        bool read_result(cc::rr::Parameter *param,
                         cc::rr::Status *status,
                         RecvFlags flags = 0) const;

        bool read_result(cc::rr::Parameter *param,
                         RecvFlags recv_flags = 0) const;

        //======================================================================
        // Invoke method  with variant request/reply parameters

        types::Value call(const std::string &method_name,
                          const types::Value &request,
                          SendFlags send_flags = 0,
                          RecvFlags recv_flags = 0) const;

        //======================================================================
        // Invoke method with ProtoBuf request/reply parameters

    private:
        void send_protobuf_invocation(const std::string method_name,
                                      const ::google::protobuf::Message &request,
                                      SendFlags send_flags) const;

        bool read_protobuf_result(types::ByteVector *bytes,
                                  RecvFlags recv_flags) const;

    public:
        template <class ResponseType = ::google::protobuf::Empty>
        ResponseType call(
            const std::string method_name,
            const ::google::protobuf::Message &request = ::google::protobuf::Empty(),
            SendFlags send_flags = 0,
            RecvFlags recv_flags = 0) const
        {
            this->send_protobuf_invocation(method_name, request, send_flags);

            types::ByteVector bytes;
            ResponseType response;
            if (this->read_protobuf_result(&bytes, recv_flags))
            {
                protobuf::to_message(bytes, &response);
                logf_trace("Received RPC ProtoBuf response: %s() -> %s", method_name, response);
            }
            else
            {
                logf_trace("Received no RPC ProtoBuf repsonse: %s()");
            }
            return response;
        }

    private:
        static uint last_client_id;
        std::string interface_name_;
        uint client_id;
        uint last_request_id;
    };
};  // namespace core::zmq
