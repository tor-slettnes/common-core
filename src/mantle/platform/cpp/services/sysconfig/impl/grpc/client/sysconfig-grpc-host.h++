// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-host.h++
/// @brief SysConfig gRPC implementation - host configuration
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "sysconfig-host.h++"
#include "sysconfig-grpc-client.h++"

namespace sysconfig::grpc
{
    class HostConfigProvider : public HostConfigInterface
    {
        using This = HostConfigProvider;
        using Super = HostConfigInterface;

    public:
        HostConfigProvider(const std::shared_ptr<Client> &client);

        void initialize() override;

        HostInfo get_host_info() const override;
        void set_host_name(const std::string &hostname) override;
        void reboot() override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace sysconfig::grpc
