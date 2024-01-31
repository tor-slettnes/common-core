/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.h++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-satellite.h++"
#include "types/bytearray.h++"

#include <optional>
#include <string>

namespace cc::zmq
{
    class Requester : public Satellite
    {
        using This = Requester;
        using Super = Satellite;

    protected:
        Requester(const std::string &host_address,
                  const std::string &channel_name);

    public:
        std::optional<types::ByteArray> send_receive(
            const types::ByteArray &request,
            ::zmq::send_flags send_flags = ::zmq::send_flags::none,
            ::zmq::recv_flags recv_flags = ::zmq::recv_flags::none);
    };

}  // namespace cc::zmq