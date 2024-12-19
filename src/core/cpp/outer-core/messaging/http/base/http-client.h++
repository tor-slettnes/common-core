/// -*- c++ -*-
//==============================================================================
/// @file http-client.h++
/// @brief HTTP requests using `libcurl`
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-utils.h++"
#include "types/valuemap.h++"
#include "logging/logging.h++"

#include <curl/curl.h>

#include <sstream>
#include <thread>

namespace core::http
{
    using URL = std::string;
    using ResponseCode = long;
    using Header = std::unordered_multimap<std::string, std::string>;

    define_log_scope("http");

    class HTTPClient
    {
        using This = HTTPClient;
        using HandleMap = std::unordered_map<std::thread::id, CURL *>;

    public:
        using ReceiveFunction = std::function<void(const char *, std::size_t)>;
        using SendFunction = std::function<std::size_t(char *, std::size_t)>;

    public:
        HTTPClient(const URL &base_url = {});
        virtual ~HTTPClient();

    public:
        virtual std::string base_url() const;
        virtual std::string url(const std::string &rel) const;

        //======================================================================
        // get()

        std::stringstream get(const std::string &location) const;

        std::stringstream get(const std::string &location,
                              const std::string &expected_content_type,
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        bool get(const std::string &location,
                 std::string *content_type,
                 std::ostream *header_stream,
                 std::ostream *content_stream,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

        bool get(const std::string &location,
                 std::string *content_type,
                 const ReceiveFunction &header_receiver,
                 const ReceiveFunction &content_receiver,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

        //======================================================================
        // put()

        std::stringstream put(const std::string &location,
                              const std::string &content_type,
                              std::istream &upload_data,
                              const std::optional<std::size_t> &upload_size,
                              const std::string &expected_content_type,
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        bool put(const std::string &location,
                 const std::string &content_type,
                 std::istream &upload_stream,
                 const std::optional<std::size_t> &upload_size,
                 std::string *received_content_type,
                 std::ostream *received_header_stream,
                 std::ostream *received_content_stream,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

        bool put(const std::string &location,
                 const std::string &content_type,
                 const SendFunction &sender,
                 const std::optional<std::size_t> &upload_size,
                 std::string *received_content_type,
                 const ReceiveFunction &header_receiver,
                 const ReceiveFunction &content_receiver,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

        //======================================================================
        // post()

        std::stringstream post(const std::string &location,
                               const std::string &content_type,
                               const std::string &data,
                               const std::string &expected_content_type,
                               bool fail_on_error = true,
                               ResponseCode *response_code = nullptr) const;

        bool post(const std::string &location,
                  const std::string &content_type,
                  const std::string &data,
                  std::string *received_content_type,
                  std::ostream *received_header_stream,
                  std::ostream *received_content_stream,
                  bool fail_on_error = false,
                  ResponseCode *response_code = nullptr) const;

        bool post(const std::string &location,
                  const std::string &content_type,
                  const std::string &data,
                  std::string *received_content_type,
                  const ReceiveFunction &header_receiver,
                  const ReceiveFunction &content_receiver,
                  bool fail_on_error = false,
                  ResponseCode *response_code = nullptr) const;

        //======================================================================
        // del()

        std::stringstream del(const std::string &location,
                              const std::string &expected_content_type,
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        bool del(const std::string &location,
                 std::string *received_content_type,
                 std::ostream *received_header_stream,
                 std::ostream *received_content_stream,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

        bool del(const std::string &location,
                 std::string *received_content_type,
                 const ReceiveFunction &receive_header_data,
                 const ReceiveFunction &receive_content_data,
                 bool fail_on_error = false,
                 ResponseCode *response_code = nullptr) const;

    protected:
        void check_content_type(
            const std::string &location,
            const std::string &received_content_type,
            const std::string &expected_content_type) const;

    private:
        static bool perform_request(
            const std::string &url,
            CURL *handle,
            CURLcode code,
            std::string *received_content_type,
            const ReceiveFunction &receive_header_data,
            const ReceiveFunction &receive_content_data,
            bool fail_on_error,
            ResponseCode *response_code);

        static ReceiveFunction stream_receiver(std::ostream *stream);
        static SendFunction stream_sender(std::istream &stream);

        static std::size_t receive(char *ptr, size_t item_size, size_t num_items, void *userdata);
        static std::size_t send(char *ptr, size_t item_size, size_t num_items, void *userdata);
        CURL *handle() const;

    private:
        std::string base_url_;
        std::mutex mtx_;
        HandleMap handles_;
    };

}  // namespace core::http
