/// -*- c++ -*-
//==============================================================================
/// @file rest-client.c++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "rest-client.h++"
#include "logging/logging.h++"
#include "parsers/json/reader.h++"
#include "parsers/json/writer.h++"

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
        const types::TaggedValueList &query,
        bool fail_on_error,
        ResponseCode *response_code) const
    {
        std::string location = join_path_query(path, query);
        return json::fast_reader.read_stream(
            this->get(location, this->content_type, fail_on_error, response_code));
    }

    types::Value RESTClient::del_json(
        const std::string &path,
        const types::TaggedValueList &query,
        bool fail_on_error,
        ResponseCode *response_code) const
    {
        std::string location = join_path_query(path, query);
        return json::fast_reader.read_stream(
            this->del(location, this->content_type, fail_on_error, response_code));
    }

    types::Value RESTClient::post_json(
        const std::string &path,
        const types::TaggedValueList &query,
        bool fail_on_error,
        ResponseCode *response_code) const
    {
        std::stringstream request;
        std::stringstream response;
        json::fast_writer.write_stream(request, query.as_kvmap());

        return json::fast_reader.read_stream(
            this->post(
                path,                // location
                this->content_type,  // content_type
                request.str(),       // data
                this->content_type,  // expected_content_type
                fail_on_error,       // fail_on_error
                response_code));     // response_code
    }
}  // namespace core::http
