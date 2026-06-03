// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-subscriber.c++
/// @brief Subscribe to producer messages and emit updates locally as signals
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-subscriber.h++"
#include "relay-zmq-handler.h++"

namespace cc::platform::pubsub::zmq
{
    Subscriber::Subscriber(const std::string &host_address,
                           const std::string &channel_name,
                           Role role)
        : cc::zmq::Subscriber(host_address, channel_name, role)
    {
    }

    void Subscriber::initialize()
    {
        cc::zmq::Subscriber::initialize();
        pubsub::Subscriber::initialize();
        this->add_handler(Handler::create_shared());
    }

    void Subscriber::deinitialize()
    {
        this->clear(true);
        pubsub::Subscriber::deinitialize();
        cc::zmq::Subscriber::deinitialize();
    }

    void Subscriber::start_reader()
    {
        cc::zmq::Subscriber::start_listening();
        pubsub::Subscriber::start_reader();
    }

    void Subscriber::stop_reader()
    {
        pubsub::Subscriber::stop_reader();
        cc::zmq::Subscriber::stop_listening();
    }
}  // namespace cc::platform::pubsub::zmq
