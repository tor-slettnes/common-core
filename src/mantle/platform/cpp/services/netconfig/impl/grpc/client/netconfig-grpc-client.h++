// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-client.h++
/// @brief NetConfig gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/protobuf/netconfig/netconfig.grpc.pb.h"  // Generated from `netconfig.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace netconfig::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<::cc::platform::netconfig::NetConfig,
                                                    ::cc::platform::netconfig::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace netconfig::grpc
