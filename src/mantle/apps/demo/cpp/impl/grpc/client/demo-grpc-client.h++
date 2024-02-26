// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-client.h++
/// @brief Demo gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo_service.grpc.pb.h"  // Generated from `demo-interface.idl`
#include "demo-api.h++"

#include "grpc-signalclient.h++"

#include "types/create-shared.h++"

namespace demo::grpc
{
    using ClientImplBase = core::grpc::SignalWatchClient<CC::Demo::Demo,
                                                       CC::Demo::Signal>;

    class ClientImpl : public demo::API,
                       public ClientImplBase,
                       public core::types::enable_create_shared<ClientImpl>
    {
        using This = ClientImpl;

    protected:
        template <class... Args>
        ClientImpl(const std::string &identity,
                   const std::string &host = "",
                   Args &&...args)
            : demo::API(identity, "gRPC Client"),
              ClientImplBase(host, std::forward<Args>(args)...)
        {
        }

        void initialize() override;

    public:
        // We keep our implementation methods private because they will
        // only be invoked via our (abstract) base API.

        void say_hello(const Greeting &greeting) override;
        TimeData get_current_time() override;
        void start_ticking() override;
        void stop_ticking() override;

        void start_watching() override;
        void stop_watching() override;
    };
}  // namespace demo::grpc
