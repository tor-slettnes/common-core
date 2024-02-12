/// -*- c++ -*-
//==============================================================================
/// @file zmq-host.h++
/// @brief Common functionality wrappers for ZeroMQ hosts/servers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"

namespace shared::zmq
{
    class Host : public Endpoint
    {
        using Super = Endpoint;

    protected:
        Host(const std::string &bind_address,
             const std::string &endpoint_type,
             const std::string &channel_name,
             ::zmq::socket_type socket_type);

    public:
        void initialize() override;

        std::string bind_address() const;

    private:
        const std::string bind_address_;
    };

}  // namespace shared::zmq
