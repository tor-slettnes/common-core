// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-grpc.c++
/// @brief Upgrade service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-providers-grpc.h++"
#include "upgrade-grpc-client.h++"
#include "upgrade-grpc-provider.h++"

namespace platform::upgrade::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);
        upgrade.registerProvider<ClientProvider>(client);

        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        upgrade.unregisterProvider<ClientProvider>();
    }
}  // namespace platform::upgrade::grpc
