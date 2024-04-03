/// -*- c++ -*-
//==============================================================================
/// @file http-client.h++
/// @brief HTTP requests using `libcurl`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-utils.h++"
#include "types/valuemap.h++"

#include <curl/curl.h>

#include <sstream>
#include <thread>

namespace core::http
{
    using URL = std::string;
    using ResponseCode = long;
    using Header = std::unordered_multimap<std::string, std::string>;


    class HTTPClient
    {
        using This = HTTPClient;
        using HandleMap = std::unordered_map<std::thread::id, CURL*>;

    public:
        HTTPClient(const URL &base_url);
        virtual ~HTTPClient();

    public:
        virtual std::string base_url() const;
        virtual std::string url(const std::string &rel) const;

        std::stringstream get(const std::string &location) const;

        std::stringstream get(const std::string &location,
                              const std::string &expected_content_type) const;

        bool get(const std::string &location,
                 ResponseCode *response_code,
                 std::string *content_type,
                 std::ostream *header_stream,
                 std::ostream *content_stream,
                 bool fail_on_error = false) const;

        ResponseCode put(const std::string &location,
                         std::ostream *stream) const;

    private:
        static size_t receive(char *ptr, size_t item_size, size_t num_items, void *userdata);
        CURL *handle() const;

    private:
        std::string base_url_;
        std::mutex mtx_;
        HandleMap handles_;
    };

}  // namespace core::http
