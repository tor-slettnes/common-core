// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-client.h++
/// @brief Vfs gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs.grpc.pb.h"  // Generated from `vfs.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace platform::vfs::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<cc::platform::vfs::VirtualFileSystem,
                                                    cc::platform::vfs::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using Super = ClientBaseImpl;

        void initialize() override;

    private:
        static void on_context(
            core::signal::MappingAction action,
            const std::string &key,
            const cc::platform::vfs::Signal &signal);

        static void on_context_in_use(
            core::signal::MappingAction action,
            const std::string &key,
            const cc::platform::vfs::Signal &signal);

    protected:
        using Super::Super;
    };

}  // namespace platform::vfs::grpc
