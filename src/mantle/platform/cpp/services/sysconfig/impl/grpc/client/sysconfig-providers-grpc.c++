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

namespace platform::sysconfig::grpc
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

        client->initialize();
        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        product.unregisterProvider<ProductProvider>();
        host.unregisterProvider<HostConfigProvider>();
        timezone.unregisterProvider<TimeZoneProvider>();
        time.unregisterProvider<TimeConfigProvider>();
    }
}  // namespace platform::sysconfig::grpc
