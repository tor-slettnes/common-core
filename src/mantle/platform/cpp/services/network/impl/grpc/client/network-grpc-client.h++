// -*- c++ -*-
//==============================================================================
/// @file network-grpc-client.h++
/// @brief Network gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network.grpc.pb.h"  // Generated from `network.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace platform::network::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<cc::network::Network,
                                                    cc::network::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace platform::network::grpc
