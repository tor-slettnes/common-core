// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-signalqueue.c++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-signalqueue.h++"
#include "demo-signals.h++"

#include "protobuf-demo-types.h++"

namespace cc::demo::grpc
{
    void SignalQueue::initialize()
    {
        using CC::Demo::Signal;

        this->connect<Greeting>(
            CC::Demo::Signal::kGreeting,
            signal_greeting,
            [](const Greeting &greeting, Signal *msg) {
                protobuf::encode(greeting, msg->mutable_greeting());
            });

        this->connect<TimeData>(
            CC::Demo::Signal::kTime,
            signal_time,
            [](const TimeData &timedata, Signal *msg) {
                protobuf::encode(timedata, msg->mutable_time());
            });
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(signal_time);
        this->disconnect(signal_greeting);
    }
}  // namespace cc::demo::grpc
