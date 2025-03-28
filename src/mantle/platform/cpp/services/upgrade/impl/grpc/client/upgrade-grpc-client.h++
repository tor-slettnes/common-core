// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-client.h++
/// @brief Upgrade gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade.grpc.pb.h"  // Generated from `upgrade.proto`
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace upgrade::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<::cc::platform::upgrade::Upgrade,
                                                    ::cc::platform::upgrade::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using Super = ClientBaseImpl;

    public:
        using Super::Super;

        void initialize() override;

    private:
        static void on_scan_progress(const ::cc::platform::upgrade::Signal &signal);
        static void on_upgrade_available(const ::cc::platform::upgrade::Signal &signal);
        static void on_upgrade_pending(const ::cc::platform::upgrade::Signal &signal);
        static void on_upgrade_progress(const ::cc::platform::upgrade::Signal &signal);
    };
}  // namespace upgrade::grpc
