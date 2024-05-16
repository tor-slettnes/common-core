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
        : Super(host_address,
                "requester",
                channel_name,
                ::zmq::socket_type::req)
    {
    }

    std::optional<types::ByteVector> Requester::send_receive(
        const types::ByteVector &request,
        ::zmq::send_flags send_flags,
        ::zmq::recv_flags recv_flags)
    {
        this->send(request, send_flags);
        return this->receive(recv_flags);
    }

}  // namespace core::zmq
