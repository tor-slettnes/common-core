/// -*- c++ -*-
//==============================================================================
/// @file rest-client.c++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rest-client.h++"
#include "json/jsondecoder.h++"

namespace core::http
{
    RESTClient::RESTClient(const std::string &base_url,
                           const std::string &service_name,
                           const std::string &content_type)
        : RESTBase("client", service_name),
          HTTPClient(this->real_url(base_url)),
          content_type(content_type)
    {
    }

    types::Value RESTClient::get_json(const std::string &location) const
    {
        std::stringstream content = this->get(location, this->content_type);
        return json::JsonDecoder::parse_text(content.str());
    }


}  // namespace core::http