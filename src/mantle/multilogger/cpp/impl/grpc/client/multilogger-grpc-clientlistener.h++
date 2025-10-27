// -*- c++ -*-
//==============================================================================
/// @file multilogger-grpc-clientlistener.h++
/// @brief Listen to log events from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-api.h++"
#include "grpc-clientwrapper.h++"
#include "types/create-shared.h++"

#include "cc/platform/multilogger/grpc/multilogger.grpc.pb.h"
#include "cc/protobuf/status/status.pb.h"

namespace multilogger::grpc
{
    //--------------------------------------------------------------------------
    // ClientListener

    class ClientListener : public LogSource,
                           public core::types::enable_create_shared<ClientListener>
    {
    protected:
        ClientListener(const std::unique_ptr<::cc::platform::multilogger::grpc::MultiLogger::Stub> &stub,
                       const ListenerSpec& request);

    public:
        ~ClientListener();

    public:
        void close() override;
        std::optional<core::types::Loggable::ptr> get() override;

    protected:
        ::grpc::ClientContext& context();

    private:
        ::grpc::ClientContext context_;
        std::unique_ptr<::grpc::ClientReader<::cc::platform::multilogger::protobuf::Loggable>> reader_;
    };
}  // namespace multilogger::grpc
