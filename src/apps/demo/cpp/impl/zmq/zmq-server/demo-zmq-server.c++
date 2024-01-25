// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-server.c++
/// @brief C++ demo - Handle ZeroMQ service requests sent as ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-server.h++"

namespace cc::demo::zmq
{
    Server::Server(const std::shared_ptr<API> &api,
                   const std::string &bind_address,
                   const std::string &channel_name)
        : Super(bind_address,
                TYPE_NAME_FULL(This),
                channel_name,
                {
                    {INTERFACE_NAME, RequestHandler::create_shared(api, INTERFACE_NAME)},
                })
    {
        // std::cerr <<  "Demo Server "
        //           << TYPE_NAME_FULL(This)
        //           << " for channel "
        //           << channel_name
        //           << " constructor."
        //           << std::endl;
    }

}  // namespace cc::demo::zmq
