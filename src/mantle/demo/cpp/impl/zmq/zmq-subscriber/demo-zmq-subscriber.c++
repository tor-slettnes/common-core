// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-subscriber.c++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-zmq-subscriber.h++"
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
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name)
        : Super(host_address, channel_name)
    {
    }

    void Subscriber::initialize()
    {
        this->add(SignalHandler::create_shared());
        Super::initialize();
    }

}  // namespace demo::zmq
