// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-subscriber.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-zmq-common.h++"
#include "zmq-protobuf-signalhandler.h++"
#include "zmq-subscriber.h++"
#include "types/create-shared.h++"

namespace cc::demo::zmq
{
    //==========================================================================
    /// @class Subscriber
    /// @brief Subscribe to and process updates from demo server
    ///
    /// Received messages are re-emitted locally via
    /// @sa cc::demo::signal_time and @sa cc::demo::signal_greeting.

    class Subscriber : public cc::zmq::Subscriber,
                       public cc::types::enable_create_shared<Subscriber>
    {
        using This = Subscriber;
        using Super = cc::zmq::Subscriber;

    protected:
        Subscriber(const std::string &host_address = "",
                   const std::string &channel_name = MESSAGE_CHANNEL);

    public:
        void initialize() override;
        void deinitialize() override;
    };

}  // namespace cc::demo::zmq
