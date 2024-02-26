/// -*- c++ -*-
//==============================================================================
/// @file dds-server.h++
/// @brief DDS RPC Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <dds/rpc/ServerParams.hpp>
#include <dds/rpc/Server.hpp>

namespace core::dds
{
    //==========================================================================
    // DDS server

    class Server : public ::dds::rpc::Server
    {
        // Convencience alias
        using This = Server;
        using Super = ::dds::rpc::Server;

    public:
        Server(uint thread_pool_size = 2);

        ~Server();

    public:
        virtual void initialize();
        virtual void deinitialize();

    private:
        ::dds::rpc::ServerParams server_params(uint thread_pool_size) const;
    };

}  // namespace core::dds
