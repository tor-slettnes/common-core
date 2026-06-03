/// -*- c++ -*-
//==============================================================================
/// @file ws-server.c++
/// @brief Base for WebSockets server implementations
/// @author Tor Slettnes
//==============================================================================

#include "ws-server.h++"

namespace cc::ws
{
    Server::Server(const std::string &address,
                   const std::string &service_name)
        : Super(address, "Server", service_name)
    {
        this->ws_server_.set_error_channels(
            websocketpp::log::elevel::all);

        this->ws_server_.set_access_channels(
            websocketpp::log::alevel::all ^
            websocketpp::log::alevel::frame_payload);
    }

    void Server::initialize()
    {
        Super::initialize();
        this->bind();
    }

    void Server::deinitialize()
    {
        Super::deinitialize();
    }

    void Server::bind()
    {
        this->ws_server_.listen(this->port());
    }

    void Server::start_listening()
    {
        if (!this->receive_thread_.joinable())
        {
            this->receive_thread_ = std::thread(&This::listen, this);
        }
    }

    void Server::stop_listening()
    {
        if (this->receive_thread_.joinable())
        {
            this->ws_server_.stop();
            this->receive_thread_.join();
        }
    }

    void Server::listen()
    {
        this->ws_server_.start_accept();
        this->ws_server_.run();
    }
}  // namespace cc::ws
