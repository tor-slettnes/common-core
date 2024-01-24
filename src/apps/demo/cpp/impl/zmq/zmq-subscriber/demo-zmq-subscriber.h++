// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-subscriber.h++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-protobuf-signalsubscriber.h++"
#include "demo-zmq-common.h++"
#include "types/create-shared.h++"

#include "demo_types.pb.h"

namespace cc::demo::zmq
{
    //==========================================================================
    /// @class Subscriber
    /// @brief Subscribe to and process updates from demo server
    ///
    /// Received messages are re-emitted locally via
    /// @sa cc::demo::signal_time and @sa cc::demo::signal_greeting.

    class Subscriber : public cc::zmq::ProtoBufSignalSubscriber<CC::Demo::Signal>,
                       public cc::types::enable_create_shared<Subscriber>
    {
        using This = Subscriber;
        using Super = cc::zmq::ProtoBufSignalSubscriber<CC::Demo::Signal>;

    protected:
        Subscriber(const std::string &channel_name = CHANNEL_NAME,
                   const std::string &host_address = {});

    public:
        void initialize() override;
    };

}  // namespace cc::demo::zmq
