// -*- c++ -*-
//==============================================================================
/// @file system-grpc-host.h++
/// @brief System service - Host Configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system-api-host.h++"
#include "system-grpc-client.h++"

namespace platform::system::grpc
{
    class HostConfig : public HostConfigProvider
    {
        using Super = HostConfigProvider;

    public:
        HostConfig(const std::shared_ptr<Client> &client);

        void initialize() override;

        HostInfo get_host_info() const override;
        void set_host_name(const std::string &hostname) override;
        void reboot() override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace platform::system::grpc
