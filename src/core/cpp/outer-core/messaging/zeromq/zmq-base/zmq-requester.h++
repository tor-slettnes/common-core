/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.h++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "types/bytevector.h++"

#include <optional>
#include <string>

namespace core::zmq
{
    class Requester : public Endpoint
    {
        using This = Requester;
        using Super = Endpoint;

    protected:
        Requester(const std::string &address,
                  const std::string &channel_name,
                  Role role = Role::SATELLITE);

    public:
        std::shared_ptr<types::ByteVector> send_receive(
            const types::ByteVector &request,
            SendFlags send_flags = 0,
            RecvFlags recv_flags = 0);
    };

}  // namespace core::zmq
