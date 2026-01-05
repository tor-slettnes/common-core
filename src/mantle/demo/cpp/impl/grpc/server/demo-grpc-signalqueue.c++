// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-signalqueue.c++
/// @brief connect server-side signals to gRPC streams initiated by clients
/// @author Tor Slettnes
//==============================================================================

#include "demo-grpc-signalqueue.h++"
#include "demo-signals.h++"

#include "protobuf-demo-types.h++"

namespace demo::grpc
{
    void SignalQueue::initialize()
    {
        using cc::demo::protobuf::Signal;

        this->connect<Greeting>(
            cc::demo::protobuf::Signal::kSignalGreeting,
            signal_greeting,
            [](const Greeting &greeting, Signal *msg) {
                ::protobuf::encode(greeting, msg->mutable_signal_greeting());
            });

        this->connect<TimeData>(
            cc::demo::protobuf::Signal::kSignalTime,
            signal_time,
            [](const TimeData &timedata, Signal *msg) {
                ::protobuf::encode(timedata, msg->mutable_signal_time());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(signal_time);
        this->disconnect(signal_greeting);
        Super::deinitialize();
    }
}  // namespace demo::grpc
