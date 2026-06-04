// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-messagequeue.h++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "grpc-serverstreamer.h++"

#include "cc/platform/pubsub/protobuf/pubsub_types.pb.h"

#include <unordered_set>

namespace cc::platform::pubsub::grpc
{
    //==========================================================================
    /// @class MessageQueue
    /// @brief Connect local signals to a single client over gRPC

    class MessageQueue
        : public cc::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Publication>
    {
        using This = MessageQueue;
        using Super = cc::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Publication>;

    public:
        MessageQueue(
            const pubsub::TopicSet &topics,
            std::size_t maxsize = 0);

        ~MessageQueue();

        void initialize();
        void deinitialize();

    private:
        void enqueue_message(
            const std::string &topic,
            const core::types::Value &value);

    private:
        std::string signal_handle_;
        std::unordered_set<std::string> topics_;

    };
}  // namespace cc::platform::pubsub::grpc
