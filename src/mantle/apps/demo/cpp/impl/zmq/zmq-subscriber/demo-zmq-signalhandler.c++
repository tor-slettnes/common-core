// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-signalhandler.h++
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
            CC::Demo::Signal::kGreeting,
            [&](const CC::Demo::Signal &signal) {
                signal_greeting.emit(
                    static_cast<shared::signal::MappingChange>(signal.change()),
                    signal.key(),
                    protobuf::decoded<Greeting>(signal.greeting()));
            });

        this->add_handler(
            CC::Demo::Signal::kTime,
            [](const CC::Demo::Signal &signal) {
                signal_time.emit(
                    protobuf::decoded<TimeData>(signal.time()));
            });

        Super::initialize();
    }

    void SignalHandler::handle_message(const CC::Demo::Signal &message)
    {
        log_debug("Handling received ProtoBuf message: ", message);
        Super::handle_message(message);
    }

}  // namespace demo::zmq
