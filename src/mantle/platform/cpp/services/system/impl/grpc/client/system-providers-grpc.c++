// -*- c++ -*-
//==============================================================================
/// @file system-providers-grpc.c++
/// @brief System service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-providers-grpc.h++"
#include "system-grpc-time.h++"
#include "system-grpc-host.h++"

namespace platform::system::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);
        time.registerProvider<Time>(client);
        hostconfig.registerProvider<HostConfig>(client);

        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        hostconfig.unregisterProvider<HostConfig>();
        time.unregisterProvider<Time>();
    }
}  // namespace platform::system::grpc
