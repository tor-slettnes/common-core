/// -*- c++ -*-
//==============================================================================
/// @file rest-client.h++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-client.h++"
#include "rest-base.h++"

namespace shared::http
{
    class RESTClient : public RESTBase,
                       public HTTPClient
    {
    public:
        RESTClient(const std::string &base_url,
                   const std::string &service_name,
                   const std::string &content_type = "application/json");

        types::Value get_json(const std::string &location) const;

    private:
        std::string content_type;
    };
}  // namespace shared::http
