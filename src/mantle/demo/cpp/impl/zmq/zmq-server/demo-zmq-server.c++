// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-server.c++
/// @brief C++ demo - Handle ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#include "demo-zmq-server.h++"
#include "demo-zmq-requesthandler.h++"

namespace demo::zmq
{
    Server::Server(const std::shared_ptr<API> &provider,
                   const std::string &bind_address,
                   const std::string &channel_name)
        : Super(bind_address,
                channel_name,
                {
                    {DEMO_INTERFACE, RequestHandler::create_shared(provider)},
                })
    {
    }

}  // namespace demo::zmq
