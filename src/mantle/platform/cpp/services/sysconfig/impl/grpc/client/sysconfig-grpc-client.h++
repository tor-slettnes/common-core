// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-client.h++
/// @brief SysConfig gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig.grpc.pb.h"  // Generated from `sysconfig.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace sysconfig::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<
        ::cc::platform::sysconfig::SysConfig,
        ::cc::platform::sysconfig::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using Super = ClientBaseImpl;

    protected:
        using Super::Super;
    };

}  // namespace sysconfig::grpc
