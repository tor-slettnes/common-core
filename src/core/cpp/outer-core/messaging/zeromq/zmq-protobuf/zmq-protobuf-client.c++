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
    ProtoBufClient::ProtoBufClient(const std::string &host_address,
                                   const std::string &channel_name,
                                   const std::string &interface_name)
        : Super(host_address, channel_name),
          interface_name_(interface_name),
          client_id(++ProtoBufClient::last_client_id),
          last_request_id(0)
    {
    }

    std::string ProtoBufClient::interface_name() const
    {
        return this->interface_name_;
    }

    void ProtoBufClient::send_request(const cc::protobuf::rr::Request &request,
                                      ::zmq::send_flags flags)
    {
        try
        {
            this->send(::cc::io::proto::to_bytes(request), flags);
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("send request", e);
        }
    }

    bool ProtoBufClient::receive_reply(cc::protobuf::rr::Reply *reply,
                                       ::zmq::recv_flags flags)
    {
        try
        {
            if (auto bytes = this->receive(flags))
            {
                return reply->ParseFromArray(bytes->data(), bytes->size());
            }
            else
            {
                return false;
            }
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("receive reply from server", e);
            return false;
        }
    }

    bool ProtoBufClient::send_receive(const cc::protobuf::rr::Request &request,
                                      cc::protobuf::rr::Reply *reply,
                                      ::zmq::send_flags send_flags,
                                      ::zmq::recv_flags recv_flags)
    {
        this->send_request(request, send_flags);
        return this->receive_reply(reply, recv_flags);
    }

    void ProtoBufClient::send_invocation(const std::string &method_name,
                                         const cc::protobuf::rr::Parameter &param,
                                         ::zmq::send_flags flags)
    {
        cc::protobuf::rr::Request request;
        request.set_client_id(this->client_id);
        request.set_request_id(++this->last_request_id);
        request.set_interface_name(this->interface_name());
        request.set_method_name(method_name);
        request.mutable_param()->CopyFrom(param);
        this->send_request(request, flags);
    }

    bool ProtoBufClient::read_result(cc::protobuf::rr::Parameter *param,
                                     cc::protobuf::rr::Status *status,
                                     ::zmq::recv_flags flags)
    {
        cc::protobuf::rr::Reply reply;
        if (this->receive_reply(&reply, flags))
        {
            status->CopyFrom(reply.status());
            param->CopyFrom(reply.param());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool ProtoBufClient::read_result(cc::protobuf::rr::Parameter *param,
                                     ::zmq::recv_flags flags)
    {
        cc::protobuf::rr::Status status;
        if (this->read_result(param, &status, flags))
        {
            switch (status.code())
            {
            case cc::protobuf::rr::STATUS_OK:
            case cc::protobuf::rr::STATUS_ACCEPTED:
                break;

            default:
                ProtoBufError(
                    status.code(),
                    ::cc::io::proto::decoded<status::Event>(status.details())
                    ).throw_if_error();
                break;
            }
            return true;
        }
        else
        {
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
        cc::protobuf::rr::Parameter request_params;
        ::cc::io::proto::encode(request, request_params.mutable_variant_value());
        this->send_invocation(method_name, request_params, send_flags);

        cc::protobuf::rr::Parameter reply_params;
        if (this->read_result(&reply_params, recv_flags))
        {
            return ::cc::io::proto::decoded<types::Value>(reply_params.variant_value());
        }
        else
        {
            return {};
        }
    }

    void ProtoBufClient::send_protobuf_invocation(const std::string method_name,
                                                  const ::google::protobuf::Message &request,
                                                  ::zmq::send_flags send_flags)
    {
        cc::protobuf::rr::Parameter request_param;
        request.SerializeToString(request_param.mutable_serialized_proto());
        this->send_invocation(method_name, request_param, send_flags);
    }

    bool ProtoBufClient::read_protobuf_result(types::ByteVector *bytes,
                                              ::zmq::recv_flags recv_flags)
    {
        cc::protobuf::rr::Parameter response_param;
        if (this->read_result(&response_param, recv_flags))
        {
            if (response_param.has_serialized_proto())
            {
                const std::string &serialized = response_param.serialized_proto();
                bytes->assign(serialized.begin(), serialized.end());
                return true;
            }
            else
            {
                return false;
            }
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
