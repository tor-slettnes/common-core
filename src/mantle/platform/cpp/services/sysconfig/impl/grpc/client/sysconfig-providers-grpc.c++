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

namespace platform::sysconfig::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);
        time.registerProvider<TimeConfigProvider>(client);
        hostconfig.registerProvider<HostConfigProvider>(client);
        timezone.registerProvider<TimeZoneProvider>(client);

        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        timezone.unregisterProvider<TimeZoneProvider>();
        hostconfig.unregisterProvider<HostConfigProvider>();
        time.unregisterProvider<TimeConfigProvider>();
    }
}  // namespace platform::sysconfig::grpc
