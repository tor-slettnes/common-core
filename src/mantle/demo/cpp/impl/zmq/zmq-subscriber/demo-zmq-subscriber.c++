// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-subscriber.c++
/// @brief Subscribe to Demo topics and emit updates locally as signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

// Application specific modules
#include "demo-zmq-subscriber.h++"
#include "demo-zmq-signalhandler.h++"
#include "protobuf-demo-types.h++"

namespace demo::zmq
{
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name)
        : Super(host_address, channel_name)
    {
    }

    void Subscriber::initialize()
    {
        Super::initialize();
        this->add_handler(SignalHandler::create_shared(), true);
    }

}  // namespace demo::zmq
