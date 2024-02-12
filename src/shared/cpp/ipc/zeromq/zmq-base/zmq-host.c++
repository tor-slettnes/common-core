/// -*- c++ -*-
//==============================================================================
/// @file zmq-host.c++
/// @brief Common functionality wrappers for ZeroMQ hosts/servers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-host.h++"
#include "logging/logging.h++"

namespace shared::zmq
{
    Host::Host(const std::string &bind_address,
               const std::string &endpoint_type,
               const std::string &channel_name,
               ::zmq::socket_type socket_type)
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
            this->socket()->bind(this->bind_address());
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not bind to socket", e);
            throw;
        }
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
}  // namespace shared::zmq
