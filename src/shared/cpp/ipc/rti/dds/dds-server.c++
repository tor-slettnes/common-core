/// -*- c++ -*-
//==============================================================================
/// @file dds-server.c++
/// @brief DDS RPC Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "dds-server.h++"
#include "application/init.h++"
#include "platform/symbols.h++"

namespace cc::dds
{
    DDS_Server::DDS_Server(const std::string &identity, uint thread_pool_size)
        : ::dds::rpc::Server(this->server_params(thread_pool_size)),
          Channel(TYPE_NAME_FULL(This), identity)
    {
    }

    void DDS_Server::initialize()
    {
        cc::application::signal_shutdown.connect(
            this->to_string(),
            std::bind(&DDS_Server::close, this));
    }

    void DDS_Server::deinitialize()
    {
        cc::application::signal_shutdown.disconnect(this->to_string());
    }

    ::dds::rpc::ServerParams DDS_Server::server_params(uint thread_pool_size) const
    {
        ::dds::rpc::ServerParams params;
        params.extensions().thread_pool_size(thread_pool_size);
        return params;
    }
}  // namespace cc::dds
