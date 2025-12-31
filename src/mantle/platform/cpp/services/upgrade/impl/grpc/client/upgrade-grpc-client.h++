// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-client.h++
/// @brief Upgrade gRPC client
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/upgrade/grpc/upgrade_service.grpc.pb.h"
#include "grpc-signalclient.h++"
#include "types/create-shared.h++"

namespace upgrade::grpc
{
    using ClientBaseImpl = core::grpc::SignalClient<::cc::platform::upgrade::grpc::Upgrade,
                                                    ::cc::platform::upgrade::protobuf::Signal>;

    class Client : public ClientBaseImpl,
                   public core::types::enable_create_shared<Client>
    {
        using This = Client;
        using Super = ClientBaseImpl;

    public:
        using Super::Super;

        void initialize() override;

    private:
        static void on_scan_progress(const ::cc::platform::upgrade::protobuf::Signal &signal);
        static void on_upgrade_available(const ::cc::platform::upgrade::protobuf::Signal &signal);
        static void on_upgrade_pending(const ::cc::platform::upgrade::protobuf::Signal &signal);
        static void on_upgrade_progress(const ::cc::platform::upgrade::protobuf::Signal &signal);
    };
}  // namespace upgrade::grpc
