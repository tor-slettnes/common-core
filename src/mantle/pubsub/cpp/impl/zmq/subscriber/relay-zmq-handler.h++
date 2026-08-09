// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-handler.h++
/// @brief Relay - ZMQ message receiver
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-subscriber.h++"
#include "types/create-shared.h++"

namespace cc::platform::pubsub::zmq
{
    class Handler : public cc::zmq::MessageHandler,
                    public core::types::enable_create_shared<Handler>
    {
        using This = Handler;
        using Super = cc::zmq::MessageHandler;

    protected:
        Handler();
        void handle(const cc::zmq::MessageParts& parts) override;
    };
}  // namespace cc::platform::pubsub::zmq
