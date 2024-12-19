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

    //======================================================================
    // get()

    std::stringstream HTTPClient::get(const std::string &location) const
    {
        std::stringstream content;
        this->get(location,  // location
                  nullptr,   // content_type
                  nullptr,   // header_stream
                  &content,  // content_stream
                  true,      // fail_on_error
                  nullptr);  // response_code
        return content;
    }

    std::stringstream HTTPClient::get(const std::string &location,
                                      const std::string &expected_content_type,
                                      bool fail_on_error,
                                      ResponseCode *response_code) const
    {
        std::string content_type;
        std::stringstream stream;
        this->get(location, &content_type, nullptr, &stream, fail_on_error, response_code);
        this->check_content_type(location, content_type, expected_content_type);
        return stream;
    }

    bool HTTPClient::get(const std::string &location,
                         std::string *content_type,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        return this->get(location,
                         content_type,
                         This::stream_receiver(header_stream),
                         This::stream_receiver(content_stream),
                         fail_on_error,
                         response_code);
    }

    bool HTTPClient::get(const std::string &location,
                         std::string *content_type,
                         const ReceiveFunction &header_receiver,
                         const ReceiveFunction &content_receiver,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        std::string url = this->url(location);
        CURL *handle = this->handle();
        CURLcode code = curl_easy_setopt(handle, CURLOPT_HTTPGET, true);

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client requesting URL: %s", url);
        }
        return This::perform_request(url,
                                     handle,
                                     code,
                                     content_type,
                                     header_receiver,
                                     content_receiver,
                                     fail_on_error,
                                     response_code);
    }

    //======================================================================
    // put()

    std::stringstream HTTPClient::put(const std::string &location,
                                      const std::string &content_type,
                                      std::istream &upload_data,
                                      const std::optional<std::size_t> &upload_size,
                                      const std::string &expected_content_type,
                                      bool fail_on_error,
                                      ResponseCode *response_code) const
    {
        std::string received_content_type;
        std::stringstream received_content_stream;
        this->put(location,                  // location
                  content_type,              // content_type
                  upload_data,               // upload_stream
                  upload_size,               // upload_size
                  &received_content_type,    // received_content_type
                  nullptr,                   // received_header_stream
                  &received_content_stream,  // received_content_stream
                  fail_on_error,             // fail_on_error
                  response_code);            // response_code

        this->check_content_type(location, received_content_type, expected_content_type);
        return received_content_stream;
    }

    bool HTTPClient::put(const std::string &location,
                         const std::string &content_type,
                         std::istream &upload_stream,
                         const std::optional<std::size_t> &upload_size,
                         std::string *received_content_type,
                         std::ostream *received_header_stream,
                         std::ostream *received_content_stream,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        return this->put(location,
                         content_type,
                         This::stream_sender(upload_stream),
                         upload_size,
                         received_content_type,
                         This::stream_receiver(received_header_stream),
                         This::stream_receiver(received_content_stream),
                         fail_on_error,
                         response_code);
    }

    bool HTTPClient::put(const std::string &location,
                         const std::string &content_type,
                         const SendFunction &sender,
                         const std::optional<std::size_t> &upload_size,
                         std::string *received_content_type,
                         const ReceiveFunction &header_receiver,
                         const ReceiveFunction &content_receiver,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        ResponseCode response = 0;
        std::string url = this->url(location);
        struct curl_slist *slist = NULL;
        CURL *handle = this->handle();
        CURLcode code = CURLE_OK;
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
            code = curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_READFUNCTION, HTTPClient::send);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_READDATA, &sender);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_INFILESIZE_LARGE, upload_size.value_or(-1));
        }

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client uploading %d bytes to URL %s", upload_size.value_or(-1), url);
        }

        try
        {
            ok = This::perform_request(url,
                                       handle,
                                       code,
                                       received_content_type,
                                       header_receiver,
                                       content_receiver,
                                       fail_on_error,
                                       response_code);
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        if (slist)
        {
            curl_slist_free_all(slist);
        }

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }

        return ok;
    }

    //======================================================================
    // post()

    std::stringstream HTTPClient::post(const std::string &location,
                                       const std::string &content_type,
                                       const std::string &data,
                                       const std::string &expected_content_type,
                                       bool fail_on_error,
                                       ResponseCode *response_code) const
    {
        std::string received_content_type;
        std::stringstream received_content_stream;
        this->post(location,                  // location
                   content_type,              // content_type
                   data,                      // data
                   &received_content_type,    // received_content_type
                   nullptr,                   // received_header_stream
                   &received_content_stream,  // received_content_stream
                   fail_on_error,             // fail_on_error
                   response_code);            // response_code

        this->check_content_type(location, received_content_type, expected_content_type);
        return received_content_stream;
    }

    bool HTTPClient::post(const std::string &location,
                          const std::string &content_type,
                          const std::string &data,
                          std::string *received_content_type,
                          std::ostream *received_header_stream,
                          std::ostream *received_content_stream,
                          bool fail_on_error,
                          ResponseCode *response_code) const
    {
        return this->post(location,
                          content_type,
                          data,
                          received_content_type,
                          This::stream_receiver(received_header_stream),
                          This::stream_receiver(received_content_stream),
                          fail_on_error,
                          response_code);
    }

    bool HTTPClient::post(const std::string &location,
                          const std::string &content_type,
                          const std::string &data,
                          std::string *received_content_type,
                          const ReceiveFunction &header_receiver,
                          const ReceiveFunction &content_receiver,
                          bool fail_on_error,
                          ResponseCode *response_code) const
    {
        ResponseCode response = 0;
        std::string url = this->url(location);
        struct curl_slist *slist = NULL;
        CURL *handle = this->handle();
        CURLcode code = CURLE_OK;
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

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client posting to URL %s: %s", url, data);
        }

        try
        {
            ok = This::perform_request(url,
                                       handle,
                                       code,
                                       received_content_type,
                                       header_receiver,
                                       content_receiver,
                                       fail_on_error,
                                       response_code);
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        if (slist)
        {
            curl_slist_free_all(slist);
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
        this->del(location, &content_type, nullptr, &stream, fail_on_error, response_code);
        this->check_content_type(location, content_type, expected_content_type);
        return stream;
    }

    bool HTTPClient::del(const std::string &location,
                         std::string *content_type,
                         std::ostream *header_stream,
                         std::ostream *content_stream,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        return this->del(location,
                         content_type,
                         This::stream_receiver(header_stream),
                         This::stream_receiver(content_stream),
                         fail_on_error,
                         response_code);
    }

    bool HTTPClient::del(const std::string &location,
                         std::string *content_type,
                         const ReceiveFunction &receive_header_data,
                         const ReceiveFunction &receive_content_data,
                         bool fail_on_error,
                         ResponseCode *response_code) const
    {
        std::string url = this->url(location);
        CURL *handle = this->handle();
        CURLcode code = curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "DELETE");

        if (code == CURLE_OK)
        {
            logf_debug("HTTP client deleting resource: %s", url);
        }

        return This::perform_request(url,                   // url
                                     handle,                // handle
                                     code,                  // code
                                     content_type,          // received_content_type
                                     receive_header_data,   // receive_header_data
                                     receive_content_data,  // receive_content_data
                                     fail_on_error,         // fail_on_error
                                     response_code);        // response_code
    }

    bool HTTPClient::perform_request(const std::string &url,
                                     CURL *handle,
                                     CURLcode code,
                                     std::string *received_content_type,
                                     const ReceiveFunction &receive_header_data,
                                     const ReceiveFunction &receive_content_data,
                                     bool fail_on_error,
                                     ResponseCode *response_code)
    {
        ResponseCode response = 0;

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HTTPClient::receive);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_HEADERDATA, &receive_header_data);
        }

        if (code == CURLE_OK)
        {
            code = curl_easy_setopt(handle, CURLOPT_WRITEDATA, &receive_content_data);
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

    HTTPClient::ReceiveFunction HTTPClient::stream_receiver(std::ostream *stream)
    {
        return [=](const char *data, std::size_t size) -> void {
            if (stream)
            {
                stream->write(data, size);
            }
        };
    }

    HTTPClient::SendFunction HTTPClient::stream_sender(std::istream &stream)
    {
        return [&](char *data, std::size_t size) -> std::size_t {
            stream.clear();
            stream.read(data, size);
            return stream.gcount();
        };
    }

    std::size_t HTTPClient::receive(char *ptr, size_t item_size, size_t num_items, void *userdata)
    {
        auto *callback = reinterpret_cast<const ReceiveFunction *>(userdata);
        std::size_t size = item_size * num_items;
        try
        {
            if (callback && *callback)
            {
                (*callback)(ptr, size);
            }
            return size;
        }
        catch (...)
        {
            logf_notice("Failed to receive %d bytes using HTTP receive function: %s",
                        size,
                        std::current_exception());
            return 0;
        }
    }

    std::size_t HTTPClient::send(char *ptr, size_t item_size, size_t num_items, void *userdata)
    {
        auto *callback = reinterpret_cast<const SendFunction *>(userdata);
        std::size_t size = item_size * num_items;
        try
        {
            if (callback && *callback)
            {
                return (*callback)(ptr, size);
            }
            else
            {
                return 0;
            }
        }
        catch (...)
        {
            logf_notice("Failed to send %d bytes using HTTP send function: %s",
                        size,
                        std::current_exception());
            return CURL_READFUNC_ABORT;
        }
    }

    CURL *HTTPClient::handle() const
    {
        auto mtx = const_cast<std::mutex *>(&this->mtx_);

        std::lock_guard lck(*mtx);
        std::thread::id thread_id = std::this_thread::get_id();
        try
        {
            // Renew an existing CURL handle
            CURL *handle = this->handles_.at(thread_id);
            curl_easy_reset(handle);
            return handle;
        }
        catch (const std::out_of_range &)
        {
            // This thread has not performed any prior requests; create a new handle.
            auto handles = const_cast<HandleMap *>(&this->handles_);
            auto [it, inserted] = handles->insert_or_assign(thread_id, curl_easy_init());
            return it->second;
        }
    }

}  // namespace core::http
