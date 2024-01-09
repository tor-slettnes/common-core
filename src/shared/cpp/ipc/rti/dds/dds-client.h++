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

    template <class Client>
    class DDS_Client : public DDS_Channel,
                       public Client
    {
    public:
        DDS_Client(
            const std::string &class_name,
            const std::string &service_name,
            int domain_id)
            : DDS_Channel(class_name, service_name, domain_id),
              Client(this->client_params())
        {
        }

    public:
        void initialize() override
        {
            application::signal_shutdown.connect(
                this->instance_name(),
                std::bind(&Client::close, this));
        }

        void deinitialize() override
        {
            application::signal_shutdown.disconnect(
                this->instance_name());
        }

    protected:
        virtual ::dds::rpc::ClientParams client_params() const
        {
            ::dds::rpc::ClientParams params(this->get_participant());
            params.service_name(this->instance_name());
            return params;
        }
    };

    //==========================================================================
    // DDS service Implementation template

    template <class ClientT>
    class DDS_Client_Wrapper
    {
    protected:
        DDS_Client_Wrapper(
            const std::string &class_name,
            const std::string &service_name,
            int domain_id)
            : client_(class_name, service_name, domain_id)
        {
        }

    public:
        inline DDS_Client<ClientT> client(
            const steady::Duration &max_wait = std::chrono::seconds(10))
        {
            logf_trace("Client %r waiting for service", this->client_.instance_name());
            this->client_.wait_for_service(max_wait);
            logf_trace("Client %r service is available", this->client_.instance_name());
            return this->client_;
        }

    protected:
        DDS_Client<ClientT> client_;
    };

}  // namespace cc::dds
