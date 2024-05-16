/// -*- c++ -*-
//==============================================================================
/// @file zmq-satellite.h++
/// @brief Common functionality wrappers for ZeroMQ satellites
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"

namespace cc::zmq
{
    class Satellite : public Endpoint
    {
        using Super = Endpoint;

    protected:
        Satellite(const std::string &host_address,
                  const std::string &endpoint_type,
                  const std::string &channel_name,
                  ::zmq::socket_type socket_type);

    public:
        void initialize() override;
        void deinitialize() override;

        virtual void connect();
        virtual void disconnect();
        std::string host_address() const;

    private:
        const std::string host_address_;
    };

}  // namespace cc::zmq
