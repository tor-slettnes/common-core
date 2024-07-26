// -*- c++ -*-
//==============================================================================
/// @file network-providers-grpc.c++
/// @brief Network service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "network-providers-grpc.h++"
#include "network-grpc-provider.h++"
#include "network-grpc-client.h++"

namespace platform::network::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);
        network.registerProvider<ClientProvider>(client);

        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        network.unregisterProvider<ClientProvider>();
    }
}  // namespace platform::network::grpc
