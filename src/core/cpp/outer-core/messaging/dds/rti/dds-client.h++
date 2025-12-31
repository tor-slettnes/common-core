/// -*- c++ -*-
//==============================================================================
/// @file dds-client.h++
/// @brief Mix-in base for DDS client implementations
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "dds-endpoint.h++"
#include "chrono/date-time.h++"
#include "types/create-shared.h++"
#include "logging/logging.h++"
#include "platform/init.h++"

#include <dds/rpc/ClientParams.hpp>

namespace core::dds
{
    //==========================================================================
    // DDS client Implementation template

    template <class BaseClient>
    class Client : public Endpoint,
                   public BaseClient
    {
        using This = Client<BaseClient>;

    public:
        Client(const std::string &service_name, int domain_id)
            : Endpoint("client", service_name, domain_id),
              BaseClient(this->client_params())
        {
        }

    public:
        void initialize() override
        {
            platform::signal_shutdown.connect(
                this->to_string(),
                std::bind(&BaseClient::close, this));
        }

        void deinitialize() override
        {
            platform::signal_shutdown.disconnect(
                this->to_string());
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
    class ClientWrapper
    {
    protected:
        ClientWrapper(const std::string &service_name, int domain_id)
            : client_(std::make_shared<Client<ClientT>>(service_name, domain_id))
        {
        }

    public:
        inline std::shared_ptr<Client<ClientT>> client(
            const steady::Duration &max_wait = std::chrono::seconds(10)) const
        {
            this->client_->wait_for_service(max_wait);
            return this->client_;
        }

    protected:
        std::shared_ptr<Client<ClientT>> client_;
    };

}  // namespace core::dds
