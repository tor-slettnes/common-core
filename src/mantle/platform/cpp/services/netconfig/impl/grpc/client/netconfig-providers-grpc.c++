// -*- c++ -*-
//==============================================================================
/// @file netconfig-providers-grpc.c++
/// @brief NetConfig service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#include "netconfig-providers-grpc.h++"
#include "netconfig-grpc-provider.h++"
#include "netconfig-grpc-client.h++"

namespace netconfig::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);
        network.registerProvider<ClientProvider>(client);

        client->initialize();
        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        network.unregisterProvider<ClientProvider>();
    }
}  // namespace netconfig::grpc
