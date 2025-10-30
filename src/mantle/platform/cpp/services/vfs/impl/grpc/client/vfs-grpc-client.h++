// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-client.h++
/// @brief Vfs gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/vfs/grpc/vfs_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace vfs::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<
        ::cc::platform::vfs::grpc::VirtualFileSystem,
        ::cc::platform::vfs::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace vfs::grpc
