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


namespace cc::platform::pubsub::grpc
{
    //==========================================================================
    // @class RequestHandler
    // @brief Process requests from Relay clients

    class RequestHandler : public cc::grpc::RequestHandler<cc::platform::pubsub::grpc::Relay>,
                           public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = cc::grpc::RequestHandler<cc::platform::pubsub::grpc::Relay>;

    public:
        ::grpc::Status Subscriber(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Filters* request,
            ::grpc::ServerWriter<platform::pubsub::protobuf::Publication>* writer) override;

        ::grpc::Status Publisher(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<platform::pubsub::protobuf::Publication>* reader,
            ::google::protobuf::Empty *reply) override;

        ::grpc::Status Publish(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Publication* message,
            ::google::protobuf::Empty *reply) override;
    };
}  // namespace cc::platform::pubsub::grpc
