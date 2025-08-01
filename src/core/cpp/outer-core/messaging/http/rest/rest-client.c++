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
#include "status/exceptions.h++"

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
        uint max_attempts,
        const core::dt::Duration &retry_interval,
        ResponseCode *response_code) const
    {
        std::string location = join_path_query(path, query);
        std::stringstream response_stream;
        std::string content_type;
        bool done = false;
        uint attempt = 0;

        for (uint attempt = 1; !done; attempt++)
        {
            try
            {
                this->get(location,          // location
                          &content_type,     // content_type
                          nullptr,           //  header_stream
                          &response_stream,  // content_stream
                          fail_on_error,     // fail_on_error,
                          response_code);    // response_code
                done = true;
            }
            catch (const exception::FailedPrecondition &e)
            {
                logf_info("HTTP request failed %d times, %d attempts remaining: %s: %s",
                          attempt,
                          max_attempts - attempt,
                          this->url(location),
                          e);

                if (attempt == max_attempts)
                {
                    throw;
                }
                else
                {
                    logf_info("Trying again in %s", retry_interval);
                    std::this_thread::sleep_for(retry_interval);
                }
            }
        }

        this->check_content_type(location, content_type, this->content_type);
        return json::fast_reader.read_stream(response_stream);
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

    types::Value RESTClient::put_json(
        const std::string &path,
        const types::TaggedValueList &query,
        const types::KeyValueMap &data,
        bool fail_on_error,
        ResponseCode *response_code) const
    {
        std::string location = join_path_query(path, query);
        return this->put_json(location, data, fail_on_error, response_code);
    }

    types::Value RESTClient::put_json(
        const std::string &path,
        const types::KeyValueMap &data,
        bool fail_on_error,
        ResponseCode *response_code) const
    {
        std::stringstream jsondata;
        std::string received_content_type;
        json::fast_writer.write_stream(jsondata, data);
        return json::fast_reader.read_stream(
            this->put(
                path,                // location
                this->content_type,  // content_type
                jsondata,            // upload_stream
                jsondata.tellp(),    // upload_size
                this->content_type,  // expected_content_type
                fail_on_error,       // fail_on_error
                response_code));     // response_code
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

}  // namespace core::http
