// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-signalqueue.h++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/pubsub/protobuf/pubsub_types.pb.h"
#include "grpc-serverstreamer.h++"

#include <unordered_set>

namespace relay::grpc
{
    //==========================================================================
    /// @class MessageQueue
    /// @brief Connect local signals to a single client over gRPC

    class MessageQueue
        : public core::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Message>
    {
        using This = MessageQueue;
        using Super = core::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Message>;

    public:
        MessageQueue(
            const std::unordered_set<std::string> &topics,
            std::size_t maxsize = 0);

        void initialize();
        void deinitialize();

        void enqueue_message(
            const std::string &topic,
            const core::types::Value &payload);

    private:
        std::unordered_set<std::string> topics_;

    };
}  // namespace relay::grpc
