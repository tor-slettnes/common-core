// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-server.h++
/// @brief Handle MultiLogger ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "multilogger-zmq-common.h++"
#include "multilogger-api.h++"
#include "zmq-protobuf-server.h++"

namespace cc::platform::multilogger::zmq
{
    class Server : public cc::zmq::ProtoBufServer,
                   public core::types::enable_create_shared<Server>
    {
        using This = Server;
        using Super = cc::zmq::ProtoBufServer;

    protected:
        Server(const std::shared_ptr<API> &provider,
               const std::string &bind_address = "");
    };

}  // namespace cc::platform::multilogger::zmq
