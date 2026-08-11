// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-requesthandler.h++
/// @brief Forward message publications over gRPC streams
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-grpc-messagequeue.h++"
#include "relay-control.h++"
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
        using MessageWriter = ::grpc::ServerWriter<platform::pubsub::protobuf::Publication>;

    public:
        RequestHandler(
            const std::shared_ptr<ControlInterface> relay_control);

        ::grpc::Status ServiceCheck(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ServiceCheckResponse *response) override;

        ::grpc::Status AssignReplayPolicies(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::ReplayPolicyMap* request,
            ::google::protobuf::Empty* reply) override;

        ::grpc::Status UnassignReplayPolicies(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Topics* request,
            ::google::protobuf::Empty* reply) override;

        ::grpc::Status GetReplayPolicies(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Topics* request,
            platform::pubsub::protobuf::ReplayPolicyMap* reply) override;

        ::grpc::Status Publisher(
            ::grpc::ServerContext* context,
            ::grpc::ServerReader<platform::pubsub::protobuf::Publication>* reader,
            ::google::protobuf::Empty* reply) override;

        ::grpc::Status Publish(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Publication* message,
            ::google::protobuf::Empty* reply) override;

        ::grpc::Status Subscriber(
            ::grpc::ServerContext* context,
            const platform::pubsub::protobuf::Filters* request,
            MessageWriter* writer) override;

    private:
        std::shared_ptr<ControlInterface> relay_control;
    };
}  // namespace cc::platform::pubsub::grpc
