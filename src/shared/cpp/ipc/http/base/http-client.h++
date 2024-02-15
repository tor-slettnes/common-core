/// -*- c++ -*-
//==============================================================================
/// @file http-client.h++
/// @brief HTTP requests using `libcurl`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "platform/init.h++"

#include <curl/curl.h>

#include <string>
#include <sstream>

namespace shared::http
{
    using URL = std::string;
    using ResponseCode = long;

    class HTTPClient
    {
        using This = HTTPClient;

    public:
        HTTPClient(const URL &base_url = "http://localhost");
        ~HTTPClient();

    public:
        static void global_init();
        static void global_cleanup();

    public:
        std::string base_url() const;
        std::stringstream get(const std::string &location) const;

        ResponseCode get(const std::string &location,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error = false) const;

        ResponseCode put(const std::string &location,
                         std::ostream *stream) const;

        static bool good_response(ResponseCode code);

    private:
        static size_t receive(char *ptr, size_t item_size, size_t num_items, void *userdata);

        // void check(CURLcode *code);

    private:
        static bool initialized_;
        std::string base_url_;
        CURL *handle_;
    };

}  // namespace shared::http
