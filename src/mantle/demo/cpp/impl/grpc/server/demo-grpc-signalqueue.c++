// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-signalqueue.c++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-signalqueue.h++"
#include "demo-signals.h++"

#include "protobuf-demo-types.h++"

namespace demo::grpc
{
    void SignalQueue::initialize()
    {
        using cc::protobuf::demo::Signal;

        this->connect<Greeting>(
            cc::protobuf::demo::Signal::kSignalGreeting,
            signal_greeting,
            [](const Greeting &greeting, Signal *msg) {
                ::cc::io::proto::encode(greeting, msg->mutable_signal_greeting());
            });

        this->connect<TimeData>(
            cc::protobuf::demo::Signal::kSignalTime,
            signal_time,
            [](const TimeData &timedata, Signal *msg) {
                ::cc::io::proto::encode(timedata, msg->mutable_signal_time());
            });
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(signal_time);
        this->disconnect(signal_greeting);
    }
}  // namespace demo::grpc
