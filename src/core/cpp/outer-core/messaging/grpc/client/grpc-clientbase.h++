/// -*- c++ -*-
//==============================================================================
/// @file grpc-clientbase.h++
/// @brief Client-side client functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-base.h++"
#include "chrono/date-time.h++"

#include <grpcpp/impl/codegen/client_context.h>

namespace core::grpc
{
    //==========================================================================
    /// @class ClientBase
    /// @brief
    ///     Wapper for client-side gRPC invocations

    class ClientBase : public Base
    {
    protected:
        ClientBase(const std::string &full_service_name,
                   const std::string &host,
                   bool wait_for_ready = false,
                   const std::shared_ptr<::grpc::ChannelCredentials> &creds =
                       ::grpc::InsecureChannelCredentials());

    private:
        std::shared_ptr<::grpc::ChannelInterface> create_channel(
            const std::shared_ptr<::grpc::ChannelCredentials> &creds) const;

    public:
        std::string host() const;

        /// @brief Check whether set_for_ready flag should be set
        bool get_wait_for_ready();

        /// @brief Set whether calls will wait for the service to be ready by default.
        /// @param[in] wait_for_ready
        ///     If set, the `wait_for_ready` flag will be set in future gRPC requests,
        ///     unless explicitly specified
        void set_wait_for_ready(bool wait_for_ready);

        /// @brief
        ///     Set gRPC call request timeout.
        /// @param[in] request_timeout
        ///     If set, the `request_timeout` will be used as default in future gRPC requests.
        void set_request_timeout(std::optional<dt::Duration> request_timeout);

        /// @brief
        ///     Indicate whether we are connected to a service.
        /// @param[in]
        ///     Attempt to connect if the current connectivity state is IDLE.
        bool connected(bool attempt = true) const;

        /// @brief Wait for service to become available.
        /// @return
        ///      `true` if server is available, `false` otherwise.
        void wait_for_connected() const;

        /// @brief Wait up to a specified duration for service to become available.
        /// @param[in] timeout
        ///     Timeout before giving up if client is currently connecting to
        ///     server. The call may block for up to this duration.
        /// @return
        ///      `true` if server is available, `false` otherwise.
        bool wait_for_connected(const dt::Duration &timeout) const;

        /// @brief Wait up to a specified duration for service to become available.
        /// @param[in] deadline
        ///     Deadline before giving up if client is currently connecting to
        ///     server.  The call may block for up to this duration.
        /// @return
        ///      `true` if server is available, `false` otherwise.
        bool wait_for_connected(const dt::TimePoint &deadline) const;


        /// @brief Check the provided gRPC status code, and throw an appropriate
        ///     exception if appropriate.
        ///
        /// @param[in] status
        ///     gRPC status code to check
        ///
        /// @throw core::grpc::ServiceError
        ///     An error response was received from the server.
        ///
        void check(const ::grpc::Status &status) const;
        void check(const Status &status) const;

    protected:
        std::string host_;
        bool wait_for_ready;
        std::optional<dt::Duration> request_timeout;
        std::shared_ptr<::grpc::ChannelInterface> channel;
    };

}  // namespace core::grpc
