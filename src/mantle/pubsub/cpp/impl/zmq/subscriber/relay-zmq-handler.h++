// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-handler.h++
/// @brief Relay - ZMQ message receiver
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-subscriber.h++"
#include "types/create-shared.h++"

namespace pubsub::zmq
{
    class Handler : public core::zmq::MessageHandler,
                    public core::types::enable_create_shared<Handler>
    {
        using This = Handler;
        using Super = core::zmq::MessageHandler;

    protected:
        Handler();
        void handle(const core::zmq::MessageParts &parts) override;
    };
}  // namespace pubsub::zmq
