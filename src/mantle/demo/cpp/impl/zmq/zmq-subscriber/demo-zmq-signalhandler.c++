// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.c++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-zmq-signalhandler.h++"
#include "demo-signals.h++"
#include "protobuf-demo-types.h++"

// Shared modules
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

// C++ STL modules
#include <functional>


namespace demo::zmq
{
    void SignalHandler::initialize()
    {
        this->add_handler(
            cc::demo::Signal::kSignalGreeting,
            [&](const cc::demo::Signal &signal) {
                signal_greeting.emit(
                    static_cast<core::signal::MappingAction>(signal.mapping_action()),
                    signal.mapping_key(),
                    ::protobuf::decoded<Greeting>(signal.signal_greeting()));
            });

        this->add_handler(
            cc::demo::Signal::kSignalTime,
            [](const cc::demo::Signal &signal) {
                signal_time.emit(
                    ::protobuf::decoded<TimeData>(signal.signal_time()));
            });

        Super::initialize();
    }

    void SignalHandler::handle_message(const cc::demo::Signal &message)
    {
        log_debug("Handling received ProtoBuf message: ", message);
        Super::handle_message(message);
    }

}  // namespace demo::zmq
