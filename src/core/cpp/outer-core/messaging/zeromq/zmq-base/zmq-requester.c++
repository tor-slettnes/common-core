/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.c++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes
//==============================================================================

#include "zmq-requester.h++"

namespace cc::zmq
{
    Requester::Requester(const std::string &address,
                         const std::string &channel_name,
                         Role role)
        : Super(address, "requester", channel_name, ZMQ_REQ, role)
    {
    }

    std::shared_ptr<core::types::ByteVector> Requester::send_receive(
        const core::types::ByteVector &request,
        SendFlags send_flags,
        RecvFlags recv_flags)
    {
        this->send(request, send_flags);
        return this->receive(recv_flags);
    }

}  // namespace cc::zmq
