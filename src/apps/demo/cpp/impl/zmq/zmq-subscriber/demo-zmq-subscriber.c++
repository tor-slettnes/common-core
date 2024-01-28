// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-subscriber.c++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-zmq-subscriber.h++"
#include "demo-signals.h++"
#include "protobuf-demo-types.h++"

// Shared modules
#include "protobuf-message.h++"
#include "protobuf-inline.h++"

// C++ STL modules
#include <functional>

namespace cc::demo::zmq
{
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name)
        : Super(host_address, channel_name)
    {
    }

    void Subscriber::initialize()
    {
        using namespace std::placeholders;

        this->add_handler(
            CC::Demo::Signal::kGreeting,
            [&](const CC::Demo::Signal &signal) {
                signal_greeting.emit(
                    static_cast<signal::MappingChange>(signal.change()),
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

}  // namespace cc::demo::zmq
