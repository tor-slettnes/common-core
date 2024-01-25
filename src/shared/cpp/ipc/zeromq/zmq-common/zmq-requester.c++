/// -*- c++ -*-
//==============================================================================
/// @file zmq-requester.c++
/// @brief Implements ZeroMQ request/reply pattern - requester
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-requester.h++"

namespace cc::zmq
{
    Requester::Requester(const std::string &host_address,
                         const std::string &class_name,
                         const std::string &channel_name)
        : Super(host_address, class_name, channel_name, ::zmq::socket_type::req)
    {
    }

    std::vector<std::string> Requester::settings_path() const
    {
        return {COMMAND_GROUP, "client"};
    }

    std::optional<types::ByteArray> Requester::send_receive(
        const types::ByteArray &request,
        ::zmq::send_flags send_flags,
        ::zmq::recv_flags recv_flags)
    {
        this->send(request, send_flags);
        return this->receive(recv_flags);
    }

}  // namespace cc::zmq
