// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-server.h++
/// @brief C++ demo - Handle ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "demo-zmq-common.h++"
#include "demo-api.h++"
#include "zmq-protobuf-server.h++"
#include "types/create-shared.h++"


namespace demo::zmq
{
    class Server : public core::zmq::ProtoBufServer,
                   public core::types::enable_create_shared<Server>
    {
        using This = Server;
        using Super = core::zmq::ProtoBufServer;

    protected:
        Server(const std::shared_ptr<API> &provider,
               const std::string &bind_address = "",
               const std::string &channel_name = SERVICE_CHANNEL);
    };

}  // namespace demo::zmq
