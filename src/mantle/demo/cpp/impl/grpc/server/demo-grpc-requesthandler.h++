// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-requesthandler.h++
/// @brief Handle DemoService RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"
#include "grpc-signalrequesthandler.h++"

#include "cc/demo/demo.grpc.pb.h"  // generated from `demo.proto`

#include "types/create-shared.h++"

namespace demo::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Demo clients

    class RequestHandler : public core::grpc::SignalRequestHandler<cc::demo::Demo>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::demo::Demo>;

    protected:
        RequestHandler(const std::shared_ptr<API>& api_provider);

    public:
        ::grpc::Status SayHello(
            ::grpc::ServerContext* context,
            const ::cc::demo::Greeting* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetCurrentTime(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::demo::TimeData* response) override;

        ::grpc::Status StartTicking(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status StopTicking(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const cc::protobuf::signal::Filter* request,
            ::grpc::ServerWriter<cc::demo::Signal>* writer) override;

    private:
        std::shared_ptr<API> provider;
    };
}  // namespace demo::grpc
