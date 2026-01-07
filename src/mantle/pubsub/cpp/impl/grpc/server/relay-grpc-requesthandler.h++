// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-requesthandler.h++
/// @brief Forward message publications over gRPC streams
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "grpc-requesthandler.h++"
#include "types/create-shared.h++"

#include "cc/platform/pubsub/grpc/relay_service.grpc.pb.h"


namespace pubsub::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    class RequestHandler : public core::grpc::RequestHandler<cc::platform::pubsub::grpc::Relay>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::platform::pubsub::grpc::Relay>;

    public:
        ::grpc::Status Subscriber(
            ::grpc::ServerContext* context,
            const ::cc::platform::pubsub::protobuf::Filters* request,
            ::grpc::ServerWriter<::cc::platform::pubsub::protobuf::Publication>* writer) override;

        ::grpc::Status Publisher(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<::cc::platform::pubsub::protobuf::Publication>* reader,
            ::google::protobuf::Empty *reply) override;

        ::grpc::Status Publish(
            ::grpc::ServerContext* context,
            const ::cc::platform::pubsub::protobuf::Publication* message,
            ::google::protobuf::Empty *reply) override;
    };
}  // namespace pubsub::grpc
