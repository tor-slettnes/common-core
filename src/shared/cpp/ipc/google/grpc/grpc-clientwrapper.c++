/// -*- c++ -*-
//==============================================================================
/// @file grpc-clientwrapper.c++
/// @brief Client-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-clientwrapper.h++"

namespace cc::grpc
{
    ClientWrapperBase::ClientWrapperBase(
        const std::string &fullServiceName,
        const std::string &host,
        bool wait_for_ready,
        const std::shared_ptr<::grpc::ChannelCredentials> &creds)
        : WrapperBase(fullServiceName),
          host_(this->realaddress(host, HOST_OPTION, PORT_OPTION, "localhost")),
          wait_for_ready(wait_for_ready)
    {
        if (int max_size = this->max_message_size())
        {
            ::grpc::ChannelArguments args;
            args.SetMaxReceiveMessageSize(max_size);
            this->channel = ::grpc::CreateCustomChannel(this->host(), creds, args);
            logf_debug(
                "Created custom channel to %s for %s, max receive size %d bytes",
                this->host(),
                this->servicename(),
                max_size);
        }
        else
        {
            this->channel = ::grpc::CreateChannel(this->host(), creds);
            logf_debug(
                "Created standard channel to %s for %s",
                this->host(),
                this->servicename());
        }
    }

    std::string ClientWrapperBase::host() const
    {
        return this->host_;
    }

    void ClientWrapperBase::check(const ::grpc::Status &status) const
    {
        if (!status.ok())
        {
            Status(status).throw_if_error();
        }
    }

    void ClientWrapperBase::check(const Status &status) const
    {
        status.throw_if_error();
    }

    bool ClientWrapperBase::get_wait_for_ready()
    {
        return this->wait_for_ready;
    }

    void ClientWrapperBase::set_wait_for_ready(bool wait_for_ready)
    {
        this->wait_for_ready = wait_for_ready;
    }

    void ClientWrapperBase::set_request_timeout(std::optional<dt::Duration> timeout)
    {
        this->request_timeout = timeout;
    }

    bool ClientWrapperBase::available(const dt::Duration &timeout) const
    {
        dt::TimePoint deadline = dt::Clock::now() + timeout;
        grpc_connectivity_state state = this->channel->GetState(true);

        while (((state == GRPC_CHANNEL_IDLE) || (state == GRPC_CHANNEL_CONNECTING)) &&
               this->channel->WaitForStateChange(state, deadline))
        {
            state = this->channel->GetState(true);
        }
        return (state == GRPC_CHANNEL_READY);
    }
}  // namespace cc::grpc
