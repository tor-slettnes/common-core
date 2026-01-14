/// -*- c++ -*-
//==============================================================================
/// @file ws-endpoint.c++
/// @brief Common functionality wrappers for WebSockets communication
/// @author Tor Slettnes
//==============================================================================

#include "ws-endpoint.h++"

#include <regex>

namespace core::ws
{
    Endpoint::Endpoint(
        const std::string &address,
        const std::string &endpoint_type,
        const std::string &channel_name)
        : messaging::Endpoint("WebSockets", endpoint_type, channel_name)
    {
        this->split_address(address,
                            &this->scheme_,
                            &this->interface_,
                            &this->port_);
    }

    std::string Endpoint::scheme() const
    {
        return !this->scheme_.empty()
            ? this->scheme_
            : this->setting(Super::SCHEME_OPTION, DEFAULT_SCHEME).as_string();
    }

    std::string Endpoint::interface() const
    {
        return !this->interface_.empty()
            ? this->interface_
            : this->setting(Super::BIND_OPTION, DEFAULT_INTERFACE).as_string();
    }

    uint Endpoint::port() const
    {
        return this->port_
            ? this->port_
            : this->setting(Super::PORT_OPTION, DEFAULT_PORT).as_uint();
    }

    void Endpoint::split_address(
        const std::string &address,
        std::string *scheme,
        std::string *host,
        uint *port)
    {
        static const std::regex rx(
            "(?:(\\w*)://)?"                       // (1) scheme
            "(\\*|\\[[\\w\\.:]*\\]|[\\w\\-\\.]*)"  // (2) host, either '[x[:x...]]' or 'n[.n]...'
            "(?::(\\d+))?"                         // (3) port
        );

        std::smatch match;
        if (std::regex_match(address, match, rx))
        {
            *scheme = match.str(1);
            *host = match.str(2);
            *port = match.length(3) ? std::stoi(match.str(3)) : 0;
        }
        else
        {
            scheme->clear();
            host->clear();
            *port = 0;
        }
    }
}  // namespace core::ws
