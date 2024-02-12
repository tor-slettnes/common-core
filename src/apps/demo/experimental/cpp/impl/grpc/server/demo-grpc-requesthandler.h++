// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.h++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"
#include "demo_service.grpc.pb.h"  // generated from `demo_service.proto`

#include "grpc-signalstream-server.h++"
#include "types/create-shared.h++"

namespace demo::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Demo clients

    class RequestHandler : public shared::grpc::SignalServer<CC::Demo::Demo>,
                           public shared::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = shared::grpc::SignalServer<CC::Demo::Demo>;

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

        ::grpc::Status stream_greetings(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::grpc::ServerWriter< ::CC::Demo::Greeting>* writer) override;

        ::grpc::Status stream_time(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::grpc::ServerWriter< ::CC::Demo::TimeData>* writer) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace demo::grpc
