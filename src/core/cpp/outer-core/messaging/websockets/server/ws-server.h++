/// -*- c++ -*-
//==============================================================================
/// @file ws-server.h++
/// @brief Base for WebSockets server implementations
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "ws-endpoint.h++"
#include <websocketpp/server.hpp>
#include <websocketpp/config/asio.hpp>
//#include <websocketpp/config/minimal_server.hpp>

namespace cc::ws
{
    class Server: public Endpoint
    {
        using This = Server;
        using Super = Endpoint;

    protected:
        Server(const std::string &address,
               const std::string &service_name);

    public:
        void initialize() override;
        void deinitialize() override;

        void bind();

        // Start background server
        virtual void start_listening();

        // Stop background server
        virtual void stop_listening();

        // Run server in foreground
        virtual void listen();

    private:
        websocketpp::server<websocketpp::config::asio> ws_server_;
        std::string address_;
        std::thread receive_thread_;
    };

}  // namespace cc::ws
