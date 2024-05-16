/// -*- c++ -*-
//==============================================================================
/// @file dds-server.c++
/// @brief DDS RPC Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-server.h++"
#include "platform/symbols.h++"
#include "platform/init.h++"

namespace cc::dds
{
    Server::Server(uint thread_pool_size)
        : ::dds::rpc::Server(this->server_params(thread_pool_size))
    {
    }

    Server::~Server()
    {
        this->close();
    }

    void Server::initialize()
    {
        cc::platform::signal_shutdown.connect(
            TYPE_NAME_FULL(This),
            std::bind(&Server::close, this));
    }

    void Server::deinitialize()
    {
        cc::platform::signal_shutdown.disconnect(TYPE_NAME_FULL(This));
    }

    ::dds::rpc::ServerParams Server::server_params(uint thread_pool_size) const
    {
        ::dds::rpc::ServerParams params;
        params.extensions().thread_pool_size(thread_pool_size);
        return params;
    }
}  // namespace cc::dds
