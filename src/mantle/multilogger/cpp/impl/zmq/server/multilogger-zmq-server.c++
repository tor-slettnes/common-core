// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-server.c++
/// @brief C++ multilogger - Handle ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#include "multilogger-zmq-server.h++"
#include "multilogger-zmq-request-handler.h++"

namespace multilogger::zmq
{
    Server::Server(const std::shared_ptr<API> &provider,
                   const std::string &bind_address)
        : Super(bind_address,
                SERVICE_CHANNEL,
                {
                    {MULTILOGGER_INTERFACE, RequestHandler::create_shared(provider)},
                })
    {
    }

}  // namespace multilogger::zmq
