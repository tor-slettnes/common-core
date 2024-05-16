// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-server.h++
/// @brief C++ demo - Handle ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-zmq-requesthandler.h++"
#include "zmq-protobuf-server.h++"

namespace demo::zmq
{
    class Server : public cc::zmq::ProtoBufServer,
                   public cc::types::enable_create_shared<Server>
    {
        using This = Server;
        using Super = cc::zmq::ProtoBufServer;

    protected:
        Server(const std::shared_ptr<API> &api,
               const std::string &bind_address = "",
               const std::string &channel_name = SERVICE_CHANNEL);

    private:
        std::shared_ptr<API> api;
    };

}  // namespace demo::zmq
