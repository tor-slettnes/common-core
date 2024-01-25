/// -*- c++ -*-
//==============================================================================
/// @file zmq-host.h++
/// @brief Common functionality wrappers for ZeroMQ hosts/servers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-base.h++"

namespace cc::zmq
{
    class Host : public Base
    {
        using Super = Base;

    protected:
        Host(const std::string &bind_address,
             const std::string &class_name,
             const std::string &channel_name,
             ::zmq::socket_type socket_type);

    public:
        void initialize() override;

        std::string bind_address() const;

    private:
        const std::string bind_address_;
    };

}  // namespace cc::zmq
