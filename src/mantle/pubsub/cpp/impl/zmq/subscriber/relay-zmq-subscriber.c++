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
        : core::zmq::Subscriber(host_address, channel_name, role)
    {
    }

    void Subscriber::initialize()
    {
        core::zmq::Subscriber::initialize();
        relay::Subscriber::initialize();
        this->add_handler(Handler::create_shared());
    }

    void Subscriber::deinitialize()
    {
        this->clear(true);
        relay::Subscriber::deinitialize();
        core::zmq::Subscriber::deinitialize();
    }

    void Subscriber::start_reader()
    {
        core::zmq::Subscriber::start_listening();
        relay::Subscriber::start_reader();
    }

    void Subscriber::stop_reader()
    {
        relay::Subscriber::stop_reader();
        core::zmq::Subscriber::stop_listening();
    }
}  // namespace relay::zmq
