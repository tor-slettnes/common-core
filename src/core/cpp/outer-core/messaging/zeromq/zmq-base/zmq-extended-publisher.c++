/// -*- c++ -*-
//==============================================================================
/// @file zmq-extended-publisher.c++
/// @brief ZMQ XPUB endpoint
/// @author Tor Slettnes
//==============================================================================

#include "zmq-extended-publisher.h++"

namespace core::zmq
{
    ExtendedPublisher::ExtendedPublisher(
        const std::string &address,
        const std::string &channel_name,
        Role role = Role::HOST)
    {
    }

}  // namespace core::zmq
