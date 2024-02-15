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

namespace shared::http
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

    void HTTPClient::global_init()
    {
        curl_global_init(CURL_GLOBAL_ALL);
    }

    void HTTPClient::global_cleanup()
    {
        curl_global_cleanup();
    }

    std::string HTTPClient::base_url() const
    {
        return this->base_url_;
    }

    std::stringstream HTTPClient::get(const std::string &location) const
    {
        std::stringstream content;
        this->get(location, nullptr, &content, true);
        return content;
    }

    ResponseCode HTTPClient::get(const std::string &location,
                                 std::ostream *header_stream,
                                 std::ostream *content_stream,
                                 bool fail_on_error) const
    {
        ResponseCode response_code = -1;

        CURLcode code = curl_easy_setopt(this->handle_, CURLOPT_URL, location.c_str());

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

        if ((code == CURLE_OK) && fail_on_error)
        {
            code = curl_easy_setopt(this->handle_, CURLOPT_FAILONERROR, 1L);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_perform(this->handle_);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_getinfo(this->handle_, CURLINFO_RESPONSE_CODE, &response_code);
        }

        if (code != CURLE_OK)
        {
            throw exception::FailedPostcondition(curl_easy_strerror(code));
        }
        return response_code;
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

    static platform::InitTask init_http_client(
        "init_http_client",
        HTTPClient::global_init);

    static platform::ExitTask cleanup_http_client(
        "cleanup_http_client",
        HTTPClient::global_cleanup);
}  // namespace shared::http
