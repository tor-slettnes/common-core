// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalwriter.c++
/// @brief Forward local DEMO signals over ZMQ
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-signalwriter.h++"
#include "demo-signals.h++"

#include "protobuf-demo-types.h++"
#include "platform/symbols.h++"

namespace demo::zmq
{
    void SignalWriter::initialize()
    {
        Super::initialize();

        // Connect local signals from `api/demo-signals.h++` to this writer.
        // Signals are captured in lambda expressions below, where we encode
        // them as 'cc::demo::Signal()` messages and then pass on to the
        // publisher using `this->write()`.

        // `signal_time` is of type `core::signal::DataSignal<TimeData>`, so
        // we receive the `TimeData()` instance as the sole argument.

        demo::signal_time.connect(
            TYPE_NAME_FULL(This),
            [=](const TimeData &timedata) {
                auto msg = this->create_signal_message();
                ::protobuf::encode(timedata, msg.mutable_signal_time());
                this->write(msg);
            });

        // `signal_greeting` is of type `core::signal::MappingSignal<Greeting>`, so
        // we receive three arguments:
        //   - The mapping action (MAP_ADDITION, MAP_UPDATE, MAP_REMOVAL)
        //   - The mapping key (in this case we use the greeter's identity)
        //   - The payload.

        demo::signal_greeting.connect(
            TYPE_NAME_FULL(This),
            [=](core::signal::MappingAction action,  // action
                const std::string &key,              // key
                const Greeting &greeting)            // payload
            {
                auto msg = this->create_signal_message(action, key);
                ::protobuf::encode(greeting, msg.mutable_signal_greeting());
                this->write(msg);
            });
    }

    void SignalWriter::deinitialize()
    {
        // We are about to be destroyed, so the callback pointers
        // will no longer remain valid. Disconnect from active signals.
        signal_greeting.disconnect(TYPE_NAME_FULL(This));
        signal_time.disconnect(TYPE_NAME_FULL(This));

        Super::deinitialize();
    }

}  // namespace demo::zmq
