/// -*- c++ -*-
//==============================================================================
/// @file grpc-clientbase.c++
/// @brief Client-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-clientbase.h++"
#include "logging/logging.h++"

namespace core::grpc
{
    ClientBase::ClientBase(
        const std::string &full_service_name,
        const std::string &host,
        bool wait_for_ready,
        const std::shared_ptr<::grpc::ChannelCredentials> &creds)
        : Base("client", full_service_name),
          host_(this->realaddress(host,
                                  HOST_OPTION,
                                  PORT_OPTION,
                                  "localhost",
                                  8080)),
          wait_for_ready(wait_for_ready),
          channel(this->create_channel(creds))
    {
    }

    std::shared_ptr<::grpc::ChannelInterface> ClientBase::create_channel(
        const std::shared_ptr<::grpc::ChannelCredentials> &creds) const
    {
        uint max_send_size = this->max_request_size();
        uint max_recv_size = this->max_reply_size();
        ::grpc::ChannelArguments args;

        if (max_send_size)
        {
            args.SetMaxSendMessageSize(max_send_size);
        }
        if (max_recv_size)
        {
            args.SetMaxReceiveMessageSize(max_recv_size);
        }

        if (max_send_size || max_recv_size)
        {
            logf_info(
                "%s creating custom channel to %s, max_send_size=%d, max_receive_size=%d",
                *this,
                this->host(),
                max_send_size,
                max_recv_size);
            return ::grpc::CreateCustomChannel(this->host(), creds, args);
        }
        else
        {
            logf_info(
                "%s creating standard channel to %s",
                *this,
                this->host());
            return ::grpc::CreateChannel(this->host(), creds);
        }
    }

    std::string ClientBase::host() const
    {
        return this->host_;
    }

    void ClientBase::check(const ::grpc::Status &status) const
    {
        if (!status.ok())
        {
            Status(status).throw_if_error();
        }
    }

    void ClientBase::check(const Status &status) const
    {
        status.throw_if_error();
    }

    bool ClientBase::get_wait_for_ready()
    {
        return this->wait_for_ready;
    }

    void ClientBase::set_wait_for_ready(bool wait_for_ready)
    {
        this->wait_for_ready = wait_for_ready;
    }

    void ClientBase::set_request_timeout(std::optional<dt::Duration> timeout)
    {
        this->request_timeout = timeout;
    }

    bool ClientBase::connected(bool attempt) const
    {
        return this->channel->GetState(attempt) == GRPC_CHANNEL_READY;
    }

    void ClientBase::wait_for_connected() const
    {
        while (!this->wait_for_connected(std::chrono::hours(1)))
        {
        }
    }

    bool ClientBase::wait_for_connected(const dt::Duration &timeout) const
    {
        return this->wait_for_connected(dt::Clock::now() + timeout);
    }

    bool ClientBase::wait_for_connected(const dt::TimePoint &deadline) const
    {
        grpc_connectivity_state state = this->channel->GetState(true);

        while (((state == GRPC_CHANNEL_IDLE) || (state == GRPC_CHANNEL_CONNECTING)) &&
               this->channel->WaitForStateChange(state, deadline))
        {
            state = this->channel->GetState(true);
        }
        return (state == GRPC_CHANNEL_READY);
    }

}  // namespace core::grpc
