/// -*- c++ -*-
//==============================================================================
/// @file dds-client.h++
/// @brief Mix-in base for DDS client implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "dds-channel.h++"
#include "platform/symbols.h++"
#include "chrono/date-time.h++"
#include "types/create-shared.h++"
#include "logging/logging.h++"
#include "application/init.h++"

#include <dds/rpc/ClientParams.hpp>

namespace cc::dds
{
    //==========================================================================
    // DDS service Implementation template

    template <class BaseClient>
    class Client : public Channel,
                   public BaseClient
    {
    public:
        Client(
            const std::string &class_name,
            const std::string &service_name,
            int domain_id)
            : Channel(class_name, service_name, domain_id),
              BaseClient(this->client_params())
        {
        }

    public:
        void initialize() override
        {
            application::signal_shutdown.connect(
                this->channel_name(),
                std::bind(&BaseClient::close, this));
        }

        void deinitialize() override
        {
            application::signal_shutdown.disconnect(
                this->channel_name());
        }

    protected:
        virtual ::dds::rpc::ClientParams client_params() const
        {
            ::dds::rpc::ClientParams params(this->get_participant());
            params.service_name(this->channel_name());
            return params;
        }
    };

    //==========================================================================
    // DDS service Implementation template

    template <class ClientT>
    class Client_Wrapper
    {
    protected:
        Client_Wrapper(
            const std::string &class_name,
            const std::string &service_name,
            int domain_id)
            : client_(class_name, service_name, domain_id)
        {
        }

    public:
        inline Client<ClientT> client(
            const steady::Duration &max_wait = std::chrono::seconds(10))
        {
            logf_trace("%s client waiting for service", this->client_.channel_name());
            this->client_.wait_for_service(max_wait);
            logf_trace("%s service is available", this->client_.channel_name());
            return this->client_;
        }

    protected:
        Client<ClientT> client_;
    };

}  // namespace cc::dds
