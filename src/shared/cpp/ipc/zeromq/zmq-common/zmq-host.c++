/// -*- c++ -*-
//==============================================================================
/// @file zmq-host.c++
/// @brief Common functionality wrappers for ZeroMQ hosts/servers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-host.h++"
#include "logging/logging.h++"

namespace cc::zmq
{
    Host::Host(const std::string &bind_address,
               const std::string &class_name,
               const std::string &channel_name,
               ::zmq::socket_type socket_type)
        : Super(class_name, channel_name, socket_type),
          bind_address_(bind_address)
    {
    }

    void Host::initialize()
    {
        Super::initialize();
        logf_debug("%s binding to %s", *this, this->bind_address());
        this->socket()->bind(this->bind_address());
    }

    std::string Host::bind_address() const
    {
        return this->realaddress(this->bind_address_,
                                 PROTOCOL_OPTION,
                                 BIND_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "*");
    }
}  // namespace cc::zmq
