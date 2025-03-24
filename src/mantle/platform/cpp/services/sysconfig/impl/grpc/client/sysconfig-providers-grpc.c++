// -*- c++ -*-
//==============================================================================
/// @file sysconfig-providers-grpc.c++
/// @brief SysConfig service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-providers-grpc.h++"
#include "sysconfig-grpc-time.h++"
#include "sysconfig-grpc-timezone.h++"
#include "sysconfig-grpc-host.h++"
#include "sysconfig-grpc-product.h++"
#include "sysconfig-grpc-process.h++"

namespace sysconfig::grpc
{
    void register_providers(const std::string &target_host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(target_host, wait_for_ready);
        time.registerProvider<TimeConfigProvider>(client);
        timezone.registerProvider<TimeZoneProvider>(client);
        host.registerProvider<HostConfigProvider>(client);
        product.registerProvider<ProductProvider>(client);
        process.registerProvider<ProcessProvider>(client);

        client->initialize();
        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        process.unregisterProvider<ProcessProvider>();
        product.unregisterProvider<ProductProvider>();
        host.unregisterProvider<HostConfigProvider>();
        timezone.unregisterProvider<TimeZoneProvider>();
        time.unregisterProvider<TimeConfigProvider>();
    }
}  // namespace sysconfig::grpc
