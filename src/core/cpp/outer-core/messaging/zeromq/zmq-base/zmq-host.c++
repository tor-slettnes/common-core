/// -*- c++ -*-
//==============================================================================
/// @file zmq-host.c++
/// @brief Common functionality wrappers for ZeroMQ hosts/servers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-host.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    Host::Host(const std::string &bind_address,
               const std::string &endpoint_type,
               const std::string &channel_name,
               SocketType socket_type)
        : Super(endpoint_type, channel_name, socket_type),
          bind_address_(bind_address)
    {
    }

    void Host::initialize()
    {
        Super::initialize();
        logf_debug("%s binding to %s", *this, this->bind_address());
        try
        {
            this->check_error(::zmq_bind(this->socket(), this->bind_address().c_str()));
        }
        catch (const Error &e)
        {
            this->log_zmq_error(
                str::format("could not bind to socket %s:", this->bind_address()),
                e);
            throw;
        }
    }

    void Host::deinitialize()
    {
        logf_debug("%s unbinding from %s", *this, this->bind_address());
        ::zmq_unbind(this->socket(), this->bind_address().c_str());
        Super::deinitialize();
    }

    std::string Host::bind_address() const
    {
        return this->realaddress(this->bind_address_,
                                 SCHEME_OPTION,
                                 BIND_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "*");
    }
}  // namespace core::zmq
