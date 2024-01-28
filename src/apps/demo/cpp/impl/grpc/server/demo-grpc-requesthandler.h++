// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.h++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-grpc-signalqueue.h++"
#include "demo-api.h++"
#include "grpc-signalservice.h++"

#include "demo_service.grpc.pb.h"  // generated from `demo_service.proto`

#include "types/create-shared.h++"

namespace cc::demo::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Demo clients

    using RequestHandlerBase = cc::grpc::SignalWatchService<CC::Demo::Demo,
                                                            CC::Demo::Signal,
                                                            SignalQueue>;

    class RequestHandler : public RequestHandlerBase,
                           public types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = RequestHandlerBase;

    protected:
        RequestHandler(const std::shared_ptr<API>& api_provider);

    public:
        ::grpc::Status say_hello(
            ::grpc::ServerContext* context,
            const ::CC::Demo::Greeting* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_current_time(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::CC::Demo::TimeData* response) override;

        ::grpc::Status start_ticking(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status stop_ticking(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace cc::demo::grpc
