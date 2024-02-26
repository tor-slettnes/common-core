/// -*- c++ -*-
//==============================================================================
/// @file http-client.h++
/// @brief HTTP requests using `cURLpp`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "http-client.h++"
#include "platform/symbols.h++"
#include "platform/init.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "thread/signaltemplate.h++"

#include <curl/curl.h>

#include <regex>

namespace core::http
{
    HTTPClient::HTTPClient(const std::string &base_url)
        : base_url_(base_url),
          handle_(curl_easy_init())
    {
    }

    HTTPClient::~HTTPClient()
    {
        curl_easy_cleanup(this->handle_);
    }

    std::string HTTPClient::base_url() const
    {
        return this->base_url_;
    }

    std::string HTTPClient::url(const std::string &rel) const
    {
        return join_urls(this->base_url(), rel);
    }

    std::stringstream HTTPClient::get(const std::string &location) const
    {
        std::stringstream content;
        this->get(location, nullptr, nullptr, nullptr, &content, true);
        return content;
    }

    std::stringstream HTTPClient::get(const std::string &location,
                                      const std::string &expected_content_type) const
    {
        std::string content_type;
        std::stringstream stream;
        this->get(location, nullptr, &content_type, nullptr, &stream, true);
        str::tolower(&content_type);
        if (!str::startswith(content_type + ";",
                             str::tolower(expected_content_type) + ";"))
        {
            throw exception::FailedPostcondition(
                "Content type mismatch",
                {
                    {"url", this->url(location)},
                    {"expected-content-type", expected_content_type},
                    {"received-content-type", content_type},
                });
        }

        return stream;
    }

    bool HTTPClient::get(const std::string &location,
                         ResponseCode *response_code,
                         std::string *content_type,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error) const
    {
        ResponseCode response = 0;
        std::string url = this->url(location);
        CURLcode code = curl_easy_setopt(this->handle_, CURLOPT_URL, url.c_str());

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(this->handle_, CURLOPT_WRITEFUNCTION, HTTPClient::receive);
        }

        if ((code == CURLE_OK) && (header_stream != nullptr))
        {
            code = curl_easy_setopt(this->handle_, CURLOPT_HEADERDATA, header_stream);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(this->handle_, CURLOPT_WRITEDATA, content_stream);
        }

        // if ((code == CURLE_OK) && fail_on_error)
        // {
        //     code = curl_easy_setopt(this->handle_, CURLOPT_FAILONERROR, 1L);
        // }

        if (code == CURLE_OK)
        {
            code = curl_easy_perform(this->handle_);
        }

        if (code != CURLE_OK)
        {
            throw exception::FailedPrecondition(
                curl_easy_strerror(code),
                {{"url", url},
                 {"curl_code", code}});
        }

        code = curl_easy_getinfo(this->handle_, CURLINFO_RESPONSE_CODE, &response);
        if (response_code)
        {
            *response_code = response;
        }

        if (content_type)
        {
            char *ctype = nullptr;
            code = curl_easy_getinfo(this->handle_, CURLINFO_CONTENT_TYPE, &ctype);
            content_type->assign(ctype ? ctype : "");
        }

        if (fail_on_error && !successful_response(response))
        {
            throwf_args(
                exception::FailedPostcondition,
                ("Server returned response code %s", response),
                {
                    {"url", url},
                    {"curl_code", code},
                    {"response_code", response},
                });
        }

        return successful_response(response);
    }

    ResponseCode HTTPClient::put(const std::string &location,
                                 std::ostream *stream) const
    {
        return 0;
    }

    size_t HTTPClient::receive(char *ptr, size_t item_size, size_t num_items, void *userdata)
    {
        auto *ss = reinterpret_cast<std::ostream *>(userdata);
        ssize_t size = item_size * num_items;
        ss->write(ptr, size);
        return ss->good() ? size : 0;
    }

}  // namespace core::http
