/// -*- c++ -*-
//==============================================================================
/// @file rest-client.h++
/// @brief Implements REST request/reply pattern - client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "http-client.h++"
#include "rest-base.h++"
#include "chrono/date-time.h++"

namespace core::http
{
    /// @brief
    ///     HTTP client that sends and receives JSON responses.
    class RESTClient : public RESTBase,
                       public HTTPClient
    {
    public:
        /// @brief
        ///     Constructor
        /// @param[in] base_url
        ///     Path relative to the `base_url` provided to the constructor
        /// @param[in] service_name
        ///     Internal name of this service, used to look up settings
        /// @param[in] messaging_flavor
        ///     Name of this messaging framework, used to look up settings.  Normally "REST".
        /// @param[in] content_type
        ///     MIME content type that we expect to receive back from the server.

        RESTClient(const std::string &base_url,
                   const std::string &service_name,
                   const std::string &messaging_flavor = "REST",
                   const std::string &content_type = "application/json");


        /// @brief
        ///     Try multiple times to send a HTTP GET query until successful, then decode the JSON response
        /// @param[in] path
        ///     Path relative to the `base_url` provided to the constructor
        /// @param[in] query
        ///     Input query.  For GET requests this is encoded in the URL.
        /// @param[in] fail_on_error
        ///     Throws an error if the server returns a non-successful HTTP
        ///     response (codes 3xx, 4xx, 5xx).
        /// @param[in] max_attempts
        ///      Maximum number of attempts to make to contact server
        /// @param[in] retry_interval
        ///      After each failed attempt, sleep for this duration before trying again. 
        /// @param[out] response_code
        ///     HTTP response code received from the server
        /// @return
        ///     Decoded JSON response from server.
        /// @exception exception::FailedPrecondition
        ///     Failed to contact server
        /// @exception exception::FailedPostCondition
        ///     Server returned an unexpected MIME type or an unsuccessful
        ///     HTTP status code.  The latter is supressed if
        ///    `fail_on_error` is `false`.

        types::Value get_json(const std::string &path,
                              const types::TaggedValueList &query,
                              bool fail_on_error,
                              uint max_attempts,
                              const core::dt::Duration &retry_interval = 5s,
                              ResponseCode *response_code = nullptr) const;

        /// @brief
        ///     Send a HTTP GET query and decode the JSON response
        /// @param[in] path
        ///     Path relative to the `base_url` provided to the constructor
        /// @param[in] query
        ///     Input query.  For GET requests this is encoded in the URL.
        /// @param[in] fail_on_error
        ///     Throws an error if the server returns a non-successful HTTP
        ///     response (codes 3xx, 4xx, 5xx).
        /// @param[out] response_code
        ///     HTTP response code received from the server
        /// @return
        ///     Decoded JSON response from server.
        /// @exception exception::FailedPrecondition
        ///     Failed to contact server
        /// @exception exception::FailedPostCondition
        ///     Server returned an unexpected MIME type or an unsuccessful
        ///     HTTP status code.  The latter is supressed if
        ///    `fail_on_error` is `false`.

        types::Value get_json(const std::string &path,
                              const types::TaggedValueList &query = {},
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        types::Value del_json(const std::string &path,
                              const types::TaggedValueList &query = {},
                              bool fail_on_error = true,
                              ResponseCode *response_code = nullptr) const;

        types::Value post_json(const std::string &path,
                               const types::TaggedValueList &query,
                               bool fail_on_error = true,
                               ResponseCode *response_code = nullptr) const;

    private:
        const std::string content_type;
    };
}  // namespace core::http
