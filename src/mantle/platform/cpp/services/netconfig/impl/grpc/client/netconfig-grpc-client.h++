// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-client.h++
/// @brief NetConfig gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/netconfig/grpc/netconfig_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace cc::platform::netconfig::grpc
{
    using ClientBaseImpl = cc::grpc::SignalClient<platform::netconfig::grpc::NetConfig,
                                                  platform::netconfig::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace cc::platform::netconfig::grpc
