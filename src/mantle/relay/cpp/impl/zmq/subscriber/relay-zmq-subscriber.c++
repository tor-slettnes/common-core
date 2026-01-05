// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-subscriber.c++
/// @brief Subscribe to producer messages and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-subscriber.h++"
#include "relay-zmq-handler.h++"

namespace relay::zmq
{
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name,
                           Role role)
        : Super(host_address, channel_name, role)
    {
    }

    void Subscriber::initialize()
    {
        Super::initialize();
        this->add_handler(Handler::create_shared());
    }

    void Subscriber::deinitialize()
    {
        this->clear(true);
        Super::deinitialize();
    }
}  // namespace relay::zmq
