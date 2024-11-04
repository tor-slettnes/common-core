// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-clientlistener.h++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-api.h++"
#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"

#include "logger.grpc.pb.h"
#include "status.pb.h"

namespace logger::grpc
{
    //--------------------------------------------------------------------------
    // ClientListener

    class ClientListener : public EventSource,
                           public core::types::enable_create_shared<ClientListener>
    {
    protected:
        ClientListener(const std::unique_ptr<::cc::logger::Logger::Stub> &stub,
                       const ListenerSpec& request);

    public:
        ~ClientListener();

    public:
        void close() override;
        std::optional<core::status::Event::ptr> get() override;

    protected:
        ::grpc::ClientContext& context();

    private:
        ::grpc::ClientContext context_;
        std::unique_ptr<::grpc::ClientReader<::cc::status::Event>> reader_;
    };
}  // namespace logger::grpc
