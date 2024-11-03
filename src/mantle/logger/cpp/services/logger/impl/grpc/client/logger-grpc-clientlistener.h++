// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-clientlistener.h++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger.grpc.pb.h"  // Generated from `logger.proto`
#include "logger-base.h++"

#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"

namespace logger::grpc
{
    using ClientListenerBase = core::grpc::ClientWrapper<cc::logger::Listener>;

    class ClientListener : public BaseListener,
                           public ClientListenerBase,
                           public core::types::enable_create_shared<ClientListener>
    {
        using This = ClientListener;

    protected:
        template <class... Args>
        ClientListener(const std::string &host = "",
                       Args &&...args)
            : BaseListener(),
              ClientListenerBase(host, std::forward<Args>(args)...)
        {
        }

    public:
        std::shared_ptr<EventQueue> listen(
            const ListenerSpec &spec) override;
    };
}  // namespace logger::grpc
