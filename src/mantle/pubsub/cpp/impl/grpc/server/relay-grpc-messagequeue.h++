// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-signalqueue.h++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-types.h++"
#include "grpc-serverstreamer.h++"

#include "cc/platform/pubsub/protobuf/pubsub_types.pb.h"

#include <unordered_set>

namespace pubsub::grpc
{
    //==========================================================================
    /// @class MessageQueue
    /// @brief Connect local signals to a single client over gRPC

    class MessageQueue
        : public core::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Publication>
    {
        using This = MessageQueue;
        using Super = core::grpc::ServerStreamer<cc::platform::pubsub::protobuf::Publication>;

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
}  // namespace pubsub::grpc
