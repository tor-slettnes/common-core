// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-client.h++
/// @brief NetConfig gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/netconfig/grpc/netconfig_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace netconfig::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<::cc::platform::netconfig::grpc::NetConfig,
                                                    ::cc::platform::netconfig::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace netconfig::grpc
