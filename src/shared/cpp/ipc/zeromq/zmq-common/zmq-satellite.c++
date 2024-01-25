/// -*- c++ -*-
//==============================================================================
/// @file zmq-satellite.c++
/// @brief Common functionality wrappers for ZeroMQ satellites
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-satellite.h++"
#include "logging/logging.h++"

namespace cc::zmq
{
    Satellite::Satellite(const std::string &host_address,
                         const std::string &class_name,
                         const std::string &channel_name,
                         ::zmq::socket_type socket_type)
        : Super(class_name, channel_name, socket_type),
          host_address_(host_address)
    {
    }

    void Satellite::initialize()
    {
        Super::initialize();
        logf_debug("%s connecting to %s", *this, this->host_address());
        this->socket()->connect(this->host_address());
    }

    void Satellite::deinitialize()
    {
        logf_debug("%s disconnecting from %s", *this, this->host_address());
        this->socket()->disconnect(this->host_address());
        Super::deinitialize();
    }

    std::string Satellite::host_address() const
    {
        return this->realaddress(this->host_address_,
                                 PROTOCOL_OPTION,
                                 CONNECT_OPTION,
                                 PORT_OPTION,
                                 "tcp",
                                 "localhost");
    }
}  // namespace cc::zmq
