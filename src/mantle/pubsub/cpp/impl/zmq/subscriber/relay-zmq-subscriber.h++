// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-subscriber.h++
/// @brief Subscribe to producer messages and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "relay-subscriber.h++"
#include "zmq-subscriber.h++"
#include "relay-zmq-common.h++"
#include "types/create-shared.h++"

namespace pubsub::zmq
{
    //==========================================================================
    /// @class Subscriber
    /// @brief Subscribe to message publications
    ///
    /// Received messages are re-emitted locally via `pubsub::signal_publication`.

    class Subscriber : public pubsub::Subscriber,
                       public core::zmq::Subscriber,
                       public core::types::enable_create_shared<Subscriber>
    {
        using This = Subscriber;

    protected:
        Subscriber(const std::string &host_address = "",
                   const std::string &channel_name = CONSUMER_CHANNEL,
                   Role role = Role::SATELLITE);

    public:
        void initialize() override;
        void deinitialize() override;

        void start_reader() override;
        void stop_reader() override;
    };

}  // namespace pubsub::zmq
