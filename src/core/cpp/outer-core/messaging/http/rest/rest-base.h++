/// -*- c++ -*-
//==============================================================================
/// @file rest-base.h++
/// @brief Implements REST request/reply pattern - base
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "messaging-endpoint.h++"

namespace core::http
{
    class RESTBase : public messaging::Endpoint
    {
    protected:
        using messaging::Endpoint::Endpoint;

    protected:
        std::string real_url(
            const std::string &partial_url,
            const std::string &scheme_option = "scheme",
            const std::string &host_option = "host",
            const std::string &port_option = "port",
            const std::string &path_option = "path",
            const std::string &default_scheme = "http",
            const std::string &default_host = "localhost",
            uint default_port = 8000,
            const std::string &default_path = "") const;

    private:
        bool spliturl(
            const std::string &url,
            std::string *scheme,
            std::string *host,
            uint *port,
            std::string *path) const;

        std::string joinurl(
            const std::string &scheme,
            const std::string &host,
            uint port,
            const std::string &path) const;
    };
}  // namespace core::http
