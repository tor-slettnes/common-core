/// -*- c++ -*-
//==============================================================================
/// @file rest-client.h++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-client.h++"
#include "rest-base.h++"

namespace core::http
{
    class RESTClient : public RESTBase,
                       public HTTPClient
    {
    public:
        RESTClient(const std::string &base_url,
                   const std::string &service_name,
                   const std::string &messaging_flavor = "REST",
                   const std::string &content_type = "application/json");

        types::Value get_json(const std::string &path,
                              const types::TaggedValueList &query = {},
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        types::Value del_json(const std::string &path,
                              const types::TaggedValueList &query = {},
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        types::Value post_json(const std::string &path,
                               const types::TaggedValueList &query,
                               bool fail_on_error = true,
                               ResponseCode *response_code = nullptr) const;

    private:
        const std::string content_type;
    };
}  // namespace core::http
