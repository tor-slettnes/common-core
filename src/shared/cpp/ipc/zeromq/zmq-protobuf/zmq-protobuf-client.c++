/// -*- c++ -*-
//==============================================================================
/// @file zmq-protobuf-client.c++
/// @brief RPC functionalty over ZeroMQ using ProtoBuf - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-protobuf-client.h++"
#include "zmq-protobuf-error.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"

namespace cc::zmq
{
    ProtoBufClient::ProtoBufClient(const std::string &class_name,
                                   const std::string &channel_name,
                                   const std::string &host_address,
                                   const std::string &interface_name)
        : Super(class_name, channel_name, host_address),
          interface_name_(interface_name),
          client_id(++ProtoBufClient::last_client_id),
          last_request_id(0)
    {
    }

    std::string ProtoBufClient::interface_name() const
    {
        return this->interface_name_;
    }

    void ProtoBufClient::send_request(const CC::RR::Request &request,
                                      ::zmq::send_flags flags)
    {
        this->send(protobuf::to_bytes(request), flags);
    }

    bool ProtoBufClient::receive_reply(CC::RR::Reply *reply,
                                       ::zmq::recv_flags flags)
    {
        ::zmq::message_t msg;
        if (this->receive(&msg, flags))
        {
            return reply->ParseFromArray(msg.data(), msg.size());
        }
        else
        {
            return false;
        }
    }

    bool ProtoBufClient::send_receive(const CC::RR::Request &request,
                                      CC::RR::Reply *reply,
                                      ::zmq::send_flags send_flags,
                                      ::zmq::recv_flags recv_flags)
    {
        this->send_request(request, send_flags);
        return this->receive_reply(reply, recv_flags);
    }

    void ProtoBufClient::send_invocation(const std::string &method_name,
                                         const CC::RR::Parameter &param,
                                         ::zmq::send_flags flags)
    {
        CC::RR::Request request;
        request.set_client_id(this->client_id);
        request.set_request_id(++this->last_request_id);
        request.set_interface_name(this->interface_name());
        request.set_method_name(method_name);
        request.mutable_param()->CopyFrom(param);
        this->send_request(request, flags);
    }

    bool ProtoBufClient::read_result(CC::RR::Parameter *param,
                                     CC::RR::Status *status,
                                     ::zmq::recv_flags flags)
    {
        CC::RR::Reply reply;
        if (this->receive_reply(&reply, flags))
        {
            status->CopyFrom(reply.status());
            param->CopyFrom(reply.param());

            switch (status->code())
            {
            case CC::RR::STATUS_OK:
            case CC::RR::STATUS_ACCEPTED:
                return true;

            default:
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    bool ProtoBufClient::read_result(CC::RR::Parameter *param,
                                     ::zmq::recv_flags flags)
    {
        CC::RR::Status status;
        if (this->read_result(param, &status, flags))
        {
            return true;
        }
        else
        {
            ProtoBufError error(
                status.code(),
                protobuf::decoded<status::Event>(status.details()));
            error.throw_if_error();
            return false;
        }
    }

    //======================================================================
    // Invoke method  with variant request/reply parameters

    types::Value ProtoBufClient::call(const std::string &method_name,
                                      const types::Value &request,
                                      ::zmq::send_flags send_flags,
                                      ::zmq::recv_flags recv_flags)
    {
        CC::RR::Parameter request_params;
        protobuf::encode(request, request_params.mutable_variant_value());
        this->send_invocation(method_name, request_params, send_flags);

        CC::RR::Parameter reply_params;
        if (this->read_result(&reply_params, recv_flags))
        {
            return protobuf::decoded<types::Value>(reply_params.variant_value());
        }
        else
        {
            return {};
        }
    }

    void ProtoBufClient::send_protobuf_invocation(const std::string method_name,
                                                  const google::protobuf::Message &request,
                                                  ::zmq::send_flags send_flags)
    {
        CC::RR::Parameter request_param;
        request.SerializeToString(request_param.mutable_serialized_proto());
        this->send_invocation(method_name, request_param, send_flags);
    }

    bool ProtoBufClient::read_protobuf_result(types::ByteArray *bytes,
                                              ::zmq::recv_flags recv_flags)
    {
        CC::RR::Parameter response_param;
        if (this->read_result(&response_param, recv_flags) &&
            response_param.has_serialized_proto())
        {
            const std::string &serialized = response_param.serialized_proto();
            bytes->assign(serialized.begin(), serialized.end());
            return true;
        }
        else
        {
            log_warning("Unable to extract serialied payload from ProtoBuf Parameter: ",
                        response_param);
            return false;
        }
    }

    uint ProtoBufClient::last_client_id = 0;
}  // namespace cc::zmq
