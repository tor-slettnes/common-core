/// -*- c++ -*-
//==============================================================================
/// @file ws-endpoint.h++
/// @brief Common functionality wrappers for WebSockets communication
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"
#include "logging/message/scope.h++"

#define ASIO_STANDALONE 1
#include <websocketpp/endpoint.hpp>

namespace cc::ws
{
    define_log_scope("ws");

    constexpr auto DEFAULT_SCHEME = "http";
    constexpr auto DEFAULT_INTERFACE = "[::]";
    constexpr auto DEFAULT_PORT = 8000;

    class Endpoint : public messaging::Endpoint
    {
        using This = Endpoint;
        using Super = messaging::Endpoint;

    protected:
        Endpoint(const std::string &address,
                 const std::string &endpoint_type,
                 const std::string &channel_name);

        std::string scheme() const;
        std::string interface() const;
        uint port() const;

    private:

        static void split_address(
            const std::string &address,
            std::string *scheme,
            std::string *host,
            uint *port);

    private:
        std::string scheme_;
        std::string interface_;
        uint port_;
    };

}  // namespace cc::ws
