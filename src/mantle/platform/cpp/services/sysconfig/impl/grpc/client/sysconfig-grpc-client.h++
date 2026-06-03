// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-client.h++
/// @brief SysConfig gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/sysconfig/grpc/sysconfig_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace cc::platform::sysconfig::grpc
{
    using ClientBaseImpl = cc::grpc::SignalClient<
        platform::sysconfig::grpc::SysConfig,
        platform::sysconfig::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace cc::platform::sysconfig::grpc
