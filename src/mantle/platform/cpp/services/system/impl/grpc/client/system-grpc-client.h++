// -*- c++ -*-
//==============================================================================
/// @file system-grpc-client.h++
/// @brief System gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system.grpc.pb.h"  // Generated from `system.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace platform::system::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<cc::platform::system::System,
                                                    cc::platform::system::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace platform::system::grpc
