/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.c++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-requester.h++"

namespace core::zmq
{
    Requester::Requester(const std::string &host_address,
                         const std::string &channel_name)
        : Super(host_address, "requester", channel_name, ZMQ_REQ)
    {
    }

    std::optional<types::ByteVector> Requester::send_receive(
        const types::ByteVector &request,
        SendFlags send_flags,
        RecvFlags recv_flags)
    {
        this->send(request, send_flags);
        return this->receive(recv_flags);
    }

}  // namespace core::zmq
