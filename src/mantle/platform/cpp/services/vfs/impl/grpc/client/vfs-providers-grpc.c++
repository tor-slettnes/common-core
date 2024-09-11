// -*- c++ -*-
//==============================================================================
/// @file vfs-providers-grpc.c++
/// @brief Vfs service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-providers-grpc.h++"
#include "vfs-grpc-provider.h++"
#include "vfs-grpc-client.h++"

namespace platform::vfs::grpc
{
    void register_providers(const std::string &host,
                            bool wait_for_ready,
                            bool start_watching)
    {
        auto client = Client::create_shared(host, wait_for_ready);

        vfs.registerProvider<ClientProvider>(client);

        client->initialize();
        if (start_watching)
        {
            client->start_watching();
        }
    }

    void unregister_providers()
    {
        vfs.unregisterProvider<ClientProvider>();
    }
}  // namespace platform::vfs::grpc
