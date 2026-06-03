// -*- c++ -*-
//==============================================================================
/// @file vfs-grpc-client.h++
/// @brief Vfs gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/vfs/grpc/vfs_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace cc::platform::vfs::grpc
{
    using ClientBaseImpl = cc::grpc::SignalClient<
        platform::vfs::grpc::VirtualFileSystem,
        platform::vfs::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace cc::platform::vfs::grpc
