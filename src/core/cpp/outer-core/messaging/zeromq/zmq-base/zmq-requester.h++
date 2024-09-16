/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.h++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-satellite.h++"
#include "types/bytevector.h++"

#include <optional>
#include <string>

namespace core::zmq
{
    class Requester : public Satellite
    {
        using This = Requester;
        using Super = Satellite;

    protected:
        Requester(const std::string &host_address,
                  const std::string &channel_name);

    public:
        std::optional<types::ByteVector> send_receive(
            const types::ByteVector &request,
            SendFlags send_flags = 0,
            RecvFlags recv_flags = 0);
    };

}  // namespace core::zmq
