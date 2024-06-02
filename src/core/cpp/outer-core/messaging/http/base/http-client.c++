/// -*- c++ -*-
//==============================================================================
/// @file http-client.c++
/// @brief HTTP requests using libcurl
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
        : base_url_(base_url)
    {
    }

    HTTPClient::~HTTPClient()
    {
        for (const auto &[thread_id, handle] : this->handles_)
        {
            curl_easy_cleanup(handle);
        }
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
                                      const std::string &expected_content_type,
                                      bool fail_on_error,
                                      ResponseCode *response_code) const
    {
        std::string content_type;
        std::stringstream stream;
        this->get(location, response_code, &content_type, nullptr, &stream, fail_on_error);
        this->check_content_type(location, content_type, expected_content_type);
        return stream;
    }

    bool HTTPClient::get(const std::string &location,
                         ResponseCode *response_code,
                         std::string *content_type,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error) const
    {
        std::string url = this->url(location);
        CURL *handle = this->handle();
        CURLcode code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_HTTPGET, true);
        }

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client requesting URL: %s", url);
        }

        return This::perform_request(url,
                                     handle,
                                     code,
                                     response_code,
                                     content_type,
                                     header_stream,
                                     content_stream,
                                     fail_on_error);
    }

    ResponseCode HTTPClient::put(const std::string &location,
                                 std::ostream *stream) const
    {
        return 0;
    }

    std::stringstream HTTPClient::post(const std::string &location,
                                       const std::string &content_type,
                                       const std::string &data,
                                       const std::string &expected_content_type,
                                       bool fail_on_error,
                                       ResponseCode *response_code) const
    {
        std::string received_content_type;
        std::stringstream received_stream;
        this->post(location,                // location
                   content_type,            // content_type
                   data,                    // data
                   response_code,           // response_code
                   &received_content_type,  // received_content_type
                   nullptr,                 // received_header_stream
                   &received_stream,        // received_content_stream
                   fail_on_error);          // fail_on_error

        this->check_content_type(location, received_content_type, expected_content_type);
        return received_stream;
    }

    bool HTTPClient::post(const std::string &location,
                          const std::string &content_type,
                          const std::string &data,
                          ResponseCode *response_code,
                          std::string *received_content_type,
                          std::ostream *received_header_stream,
                          std::ostream *received_content_stream,
                          bool fail_on_error) const
    {
        ResponseCode response = 0;
        std::string url = this->url(location);
        struct curl_slist *slist = NULL;
        CURL *handle = this->handle();
        CURLcode code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        std::exception_ptr eptr = nullptr;
        bool ok = false;

        if (code == CURLE_OK)
        {
            std::string header = "Content-Type: " + content_type;
            slist = curl_slist_append(slist, header.data());
            code = curl_easy_setopt(handle, CURLOPT_HTTPHEADER, slist);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.data());
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data.size());
        }

        try
        {
            if (code == CURLE_OK)
            {
                logf_debug("HTTP client posting to URL: %s", url);
            }

            ok = This::perform_request(url,
                                       handle,
                                       code,
                                       response_code,
                                       received_content_type,
                                       received_header_stream,
                                       received_content_stream,
                                       fail_on_error);
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        if (slist)
        {
            curl_slist_free_all(slist); /* free the list again */
        }

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }

        return ok;
    }

    std::stringstream HTTPClient::del(const std::string &location,
                                      const std::string &expected_content_type,
                                      bool fail_on_error,
                                      ResponseCode *response_code) const
    {
        std::string content_type;
        std::stringstream stream;
        this->del(location, response_code, &content_type, nullptr, &stream, fail_on_error);
        this->check_content_type(location, content_type, expected_content_type);
        return stream;
    }

    bool HTTPClient::del(const std::string &location,
                         ResponseCode *response_code,
                         std::string *content_type,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error) const
    {
        std::string url = this->url(location);
        CURL *handle = this->handle();
        CURLcode code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");
        }

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client deleting resource: %s", url);
        }

        return This::perform_request(url,
                                     handle,
                                     code,
                                     response_code,
                                     content_type,
                                     header_stream,
                                     content_stream,
                                     fail_on_error);
    }

    bool HTTPClient::perform_request(const std::string &url,
                                     CURL *handle,
                                     CURLcode code,
                                     ResponseCode *response_code,
                                     std::string *received_content_type,
                                     std::ostream *received_header_stream,
                                     std::ostream *received_content_stream,
                                     bool fail_on_error)
    {
        ResponseCode response = 0;

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HTTPClient::receive);
        }

        if ((code == CURLE_OK) && (received_header_stream != nullptr))
        {
            code = curl_easy_setopt(handle, CURLOPT_HEADERDATA, received_header_stream);
        }

        if ((code == CURLE_OK) && (received_content_stream != nullptr))
        {
            code = curl_easy_setopt(handle, CURLOPT_WRITEDATA, received_content_stream);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_perform(handle);
        }

        if (code != CURLE_OK)
        {
            throw exception::FailedPrecondition(
                curl_easy_strerror(code),
                {{"url", url},
                 {"curl_code", code}});
        }

        code = curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &response);
        logf_debug("Received response code: %s", response);
        if (response_code)
        {
            *response_code = response;
        }

        if (received_content_type)
        {
            char *ctype = nullptr;
            code = curl_easy_getinfo(handle, CURLINFO_CONTENT_TYPE, &ctype);
            received_content_type->assign(ctype ? ctype : "");
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

    void HTTPClient::check_content_type(const std::string &location,
                                        const std::string &received_content_type,
                                        const std::string &expected_content_type) const
    {
        std::string content_type = str::tolower(received_content_type);
        if (!str::startswith(content_type + ";",
                             str::tolower(expected_content_type) + ";"))
        {
            throw exception::FailedPostcondition(
                "Content type mismatch",
                {
                    {"url", this->url(location)},
                    {"received-content-type", received_content_type},
                    {"expected-content-type", expected_content_type},
                });
        }
    }

    size_t HTTPClient::receive(char *ptr, size_t item_size, size_t num_items, void *userdata)
    {
        auto *ss = reinterpret_cast<std::ostream *>(userdata);
        ssize_t size = item_size * num_items;
        logf_trace("HTTP client received %d bytes from server", size);
        ss->write(ptr, size);
        return ss->good() ? size : 0;
    }

    CURL *HTTPClient::handle() const
    {
        auto mtx = const_cast<std::mutex *>(&this->mtx_);

        std::lock_guard lck(*mtx);
        std::thread::id thread_id = std::this_thread::get_id();
        try
        {
            return this->handles_.at(thread_id);
        }
        catch (const std::out_of_range &)
        {
            auto handles = const_cast<HandleMap *>(&this->handles_);
            auto [it, inserted] = handles->insert_or_assign(thread_id, curl_easy_init());
            return it->second;
        }
    }

}  // namespace core::http
