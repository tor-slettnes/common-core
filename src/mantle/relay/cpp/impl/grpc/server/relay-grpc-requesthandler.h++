// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-requesthandler.h++
/// @brief Message forwarding over gRPC streams
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "grpc-requesthandler.h++"
#include "types/create-shared.h++"

#include "cc/platform/relay/grpc/relay_service.grpc.pb.h"


namespace relay::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    class RequestHandler : public core::grpc::RequestHandler<cc::platform::relay::grpc::Relay>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::platform::relay::grpc::Relay>;

    public:
        ::grpc::Status Reader(
            ::grpc::ServerContext* context,
            const ::cc::platform::relay::protobuf::Filters* request,
            ::grpc::ServerWriter<::cc::platform::relay::protobuf::Message>* writer) override;

        ::grpc::Status Writer(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<::cc::platform::relay::protobuf::Message>* reader,
            ::google::protobuf::Empty *reply) override;

        ::grpc::Status Publish(
            ::grpc::ServerContext* context,
            const ::cc::platform::relay::protobuf::Message* message,
            ::google::protobuf::Empty *reply) override;
    };
}  // namespace relay::grpc
