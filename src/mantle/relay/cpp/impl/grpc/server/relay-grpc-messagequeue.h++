// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-signalqueue.h++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/relay/protobuf/relay_types.pb.h"
#include "grpc-serverstreamer.h++"
#include "thread/signaltemplate.h++"

#include <unordered_set>

namespace relay::grpc
{
    //==========================================================================
    /// @class MessageQueue
    /// @brief Connect local signals to a single client over gRPC

    class MessageQueue
        : public core::grpc::ServerStreamer<cc::platform::relay::protobuf::Message>
    {
        using This = MessageQueue;
        using Super = core::grpc::ServerStreamer<cc::platform::relay::protobuf::Message>;

    public:
        MessageQueue(
            const std::unordered_set<std::string> &topics,
            std::size_t maxsize = 0);

        void initialize();
        void deinitialize();

        void enqueue_message(
            core::signal::MappingAction action,
            const std::string &topic,
            const core::types::Value &payload);

    private:
        std::unordered_set<std::string> topics_;

    };
}  // namespace relay::grpc
