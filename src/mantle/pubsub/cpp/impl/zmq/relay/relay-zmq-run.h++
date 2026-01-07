// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-run.h++
/// @brief Message Relay - ZeroMQ listener initialization
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <memory>
#include <string>

namespace pubsub::zmq
{
    void run_zmq_listeners(
        const std::string &producer_interface,
        const std::string &consumer_interface);
}
