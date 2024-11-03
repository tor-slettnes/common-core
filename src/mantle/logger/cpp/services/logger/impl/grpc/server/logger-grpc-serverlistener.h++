// -*- c++ -*-
//==============================================================================
/// @file logger-grpc-serverlistener.h++
/// @brief Stream log message from server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-base.h++"
#include "grpc-requesthandler.h++"

#include "logger.grpc.pb.h"  // generated from `logger.proto`

#include "types/create-shared.h++"

namespace logger::grpc
{
    class ServerListener : public core::grpc::RequestHandler<cc::logger::Listener>,
                           public core::types::enable_create_shared<ServerListener>
    {
        using This = ServerListener;
        using Super = core::grpc::RequestHandler<cc::logger::Listener>;

    protected:
        ServerListener(const std::shared_ptr<BaseListener>& provider);

    public:
        ::grpc::Status listen(
            ::grpc::ServerContext* context,
            const ::cc::logger::ListenerSpec* request,
            ::grpc::ServerWriter<::cc::status::Event>* writer) override;

    private:
        std::shared_ptr<BaseListener> provider;
    };
}  // namespace logger::grpc
