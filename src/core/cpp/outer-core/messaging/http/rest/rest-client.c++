/// -*- c++ -*-
//==============================================================================
/// @file rest-client.c++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rest-client.h++"
#include "logging/logging.h++"

namespace core::http
{
    RESTClient::RESTClient(
        const std::string &base_url,
        const std::string &service_name,
        const std::string &messaging_flavor,
        const std::string &content_type)
        : RESTBase(messaging_flavor, "client", service_name),
          HTTPClient(this->real_url(base_url)),
          content_type(content_type)
    {
    }

    types::Value RESTClient::get_json(
        const std::string &path,
        const types::TaggedValueList &query) const
    {
        std::string location = join_path_query(path, query);
        std::stringstream ss{this->get(location, this->content_type)};
        logf_debug("REST client received %d bytes; parsing JSON...", ss.tellp());
        types::Value value = json::fast_reader.read_stream(ss);
        log_debug("REST client parsed JSON response.");
        return value;
    }

}  // namespace core::http
