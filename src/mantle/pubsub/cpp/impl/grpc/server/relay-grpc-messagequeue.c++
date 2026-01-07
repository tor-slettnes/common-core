// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-signalqueue.c++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-messagequeue.h++"
#include "relay-types.h++"
#include "protobuf-variant-types.h++"

namespace pubsub::grpc
{
    MessageQueue::MessageQueue(
        const pubsub::TopicSet &topics,
        std::size_t maxsize)
        : Super(maxsize),
          topics_(topics)
    {
    }

    MessageQueue::~MessageQueue()
    {
        this->deinitialize();
    }

    void MessageQueue::initialize()
    {
        using namespace std::placeholders;

        this->signal_handle_ = pubsub::signal_publication.connect(
            std::bind(&This::enqueue_message, this, _2, _3));
    }

    void MessageQueue::deinitialize()
    {
        pubsub::signal_publication.disconnect(
            this->signal_handle_);
    }

    void MessageQueue::enqueue_message(
        const std::string &topic,
        const core::types::Value &payload)
    {
        if (this->topics_.empty() || this->topics_.count(topic))
        {
            cc::platform::pubsub::protobuf::Publication msg;
            msg.set_topic(topic);
            protobuf::encode(payload, msg.mutable_payload());
            this->put(std::move(msg));
        }
    }
}  // namespace pubsub::grpc
