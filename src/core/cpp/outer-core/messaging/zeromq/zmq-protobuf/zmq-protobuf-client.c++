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

namespace core::zmq
{
    ProtoBufClient::ProtoBufClient(const std::string &address,
                                   const std::string &channel_name,
                                   const std::string &interface_name,
                                   Role role)
        : Super(address, channel_name, role),
          interface_name_(interface_name),
          client_id(++ProtoBufClient::last_client_id),
          last_request_id(0)
    {
    }

    std::string ProtoBufClient::interface_name() const
    {
        return this->interface_name_;
    }

    void ProtoBufClient::send_request(const cc::protobuf::request_reply::Request &request,
                                      SendFlags flags) const
    {
        try
        {
            this->send(::protobuf::to_bytes(request), flags);
        }
        catch (const Error &e)
        {
            this->log_zmq_error("send request", e);
        }
    }

    bool ProtoBufClient::receive_reply(cc::protobuf::request_reply::Reply *reply,
                                       RecvFlags flags) const
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
        catch (const Error &e)
        {
            this->log_zmq_error("receive reply from server", e);
            return false;
        }
    }

    bool ProtoBufClient::send_receive(const cc::protobuf::request_reply::Request &request,
                                      cc::protobuf::request_reply::Reply *reply,
                                      SendFlags send_flags,
                                      RecvFlags recv_flags) const
    {
        this->send_request(request, send_flags);
        return this->receive_reply(reply, recv_flags);
    }

    uint ProtoBufClient::next_request_id() const
    {
        return ++const_cast<ProtoBufClient *>(this)->last_request_id;
    }

    void ProtoBufClient::send_invocation(const std::string &method_name,
                                         const cc::protobuf::request_reply::Parameter &param,
                                         SendFlags flags) const
    {
        cc::protobuf::request_reply::Request request;
        request.set_client_id(this->client_id);
        request.set_request_id(this->next_request_id());
        request.set_interface_name(this->interface_name());
        request.set_method_name(method_name);
        request.mutable_param()->CopyFrom(param);
        this->send_request(request, flags);
    }

    bool ProtoBufClient::read_result(cc::protobuf::request_reply::Parameter *param,
                                     cc::protobuf::request_reply::Status *status,
                                     RecvFlags flags) const
    {
        cc::protobuf::request_reply::Reply reply;
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

    bool ProtoBufClient::read_result(cc::protobuf::request_reply::Parameter *param,
                                     RecvFlags flags) const
    {
        cc::protobuf::request_reply::Status status;
        if (this->read_result(param, &status, flags))
        {
            switch (status.code())
            {
            case cc::protobuf::request_reply::STATUS_OK:
            case cc::protobuf::request_reply::STATUS_ACCEPTED:
                break;

            default:
                ProtoBufError(
                    status.code(),
                    ::protobuf::decoded<status::Error>(status.details()))
                    .throw_if_error();
                break;
            }
            return true;
        }
        else
        {
            return false;
        }
    }

    //==========================================================================
    // Invoke method  with variant request/reply parameters

    types::Value ProtoBufClient::call(const std::string &method_name,
                                      const types::Value &request,
                                      SendFlags send_flags,
                                      RecvFlags recv_flags) const
    {
        cc::protobuf::request_reply::Parameter request_params;
        ::protobuf::encode(request, request_params.mutable_variant_value());

        logf_trace("Invoking RPC with value: %s(%s)", method_name, request);
        this->send_invocation(method_name, request_params, send_flags);

        cc::protobuf::request_reply::Parameter reply_params;
        if (this->read_result(&reply_params, recv_flags))
        {
            auto response = ::protobuf::decoded<types::Value>(
                reply_params.variant_value());

            logf_trace("Received RPC response: %s() -> %s",
                       method_name,
                       response);

            return response;
        }
        else
        {
            return {};
        }
    }

    void ProtoBufClient::send_protobuf_invocation(const std::string method_name,
                                                  const ::google::protobuf::Message &request,
                                                  SendFlags send_flags) const
    {
        cc::protobuf::request_reply::Parameter request_param;

        logf_trace("Invoking RPC with ProtoBuf input: %s(%s)", method_name, request);
        request.SerializeToString(request_param.mutable_serialized_proto());
        this->send_invocation(method_name, request_param, send_flags);
    }

    bool ProtoBufClient::read_protobuf_result(types::ByteVector *bytes,
                                              RecvFlags recv_flags) const
    {
        cc::protobuf::request_reply::Parameter response_param;
        if (this->read_result(&response_param, recv_flags))
        {
            switch (response_param.param_case())
            {
            case cc::protobuf::request_reply::Parameter::ParamCase::kSerializedProto:
            {
                const std::string &serialized = response_param.serialized_proto();
                bytes->assign(serialized.begin(), serialized.end());
                return true;
            }
            default:
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
}  // namespace core::zmq
