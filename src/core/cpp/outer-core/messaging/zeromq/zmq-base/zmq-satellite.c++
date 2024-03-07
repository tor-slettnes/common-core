/// -*- c++ -*-
//==============================================================================
/// @file zmq-satellite.c++
/// @brief Common functionality wrappers for ZeroMQ satellites
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-satellite.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace core::zmq
{
    Satellite::Satellite(const std::string &host_address,
                         const std::string &endpoint_type,
                         const std::string &channel_name,
                         ::zmq::socket_type socket_type)
        : Super(endpoint_type, channel_name, socket_type),
          host_address_(host_address)
    {
    }

    void Satellite::initialize()
    {
        Super::initialize();
        this->connect();
    }

    void Satellite::deinitialize()
    {
        this->disconnect();
        Super::deinitialize();
    }

    void Satellite::connect()
    {
        logf_debug("%s connecting to %s", *this, this->host_address());
        this->socket()->connect(this->host_address());
    }

    void Satellite::disconnect()
    {
        try
        {
            logf_debug("%s disconnecting from %s", *this, this->host_address());
            this->socket()->disconnect(this->host_address());
        }
        catch (const ::zmq::error_t &e)
        {
            this->log_zmq_error("could not disconnect from " + this->host_address(), e);
        }
    }

    std::string Satellite::host_address() const
    {
        return this->realaddress(this->host_address_,
                                 SCHEME_OPTION,
                                 CONNECT_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "localhost");
    }
}  // namespace core::zmq
