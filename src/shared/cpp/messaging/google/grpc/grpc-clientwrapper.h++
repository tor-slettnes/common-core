/// -*- c++ -*-
//==============================================================================
/// @file grpc-clientwrapper.h++
/// @brief Client-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for Common Core gRPC services (client and server), including:
///  * Settings store in YourServiceName.json, using a JSON backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-base.h++"
#include "logging/logging.h++"
#include "chrono/date-time.h++"

#include <google/protobuf/empty.pb.h>
#include <grpcpp/impl/codegen/sync_stream.h>
#include <grpcpp/impl/codegen/client_context.h>

namespace shared::grpc
{
    //==========================================================================
    /// @class ClientBase
    /// @brief
    ///     Wapper for client-side gRPC invocations

    class ClientWrapperBase : public Base
    {
    protected:
        ClientWrapperBase(const std::string &full_service_name,
                          const std::string &host,
                          bool wait_for_ready = false,
                          const std::shared_ptr<::grpc::ChannelCredentials> &creds =
                              ::grpc::InsecureChannelCredentials());

    private:
        std::shared_ptr<::grpc::ChannelInterface> create_channel(
            const std::shared_ptr<::grpc::ChannelCredentials> &creds) const;

    public:
        std::string host() const;

        /// @fn get_wait_for_ready
        /// @brief Check whether set_for_ready flag should be set
        bool get_wait_for_ready();

        /// @fn set_wait_for_ready
        /// @brief Set whether calls will wait for the service to be ready by default.
        /// @param[in] wait_for_ready
        ///     If set, the `wait_for_ready` flag will be set in future gRPC requests,
        ///     unless explicitly specified
        void set_wait_for_ready(bool wait_for_ready);

        /// @fn set_request_timeout
        /// @brief Set gRPC call request timeout in ms.
        /// @param[in] request_timeout
        ///     If set, the `request_timeout` will be set in future gRPC requests,
        ///     unless explicitly specified.
        void set_request_timeout(std::optional<dt::Duration> request_timeout);

        /// @fn ready
        /// @brief Determine whether the service is available
        /// @param[in] timeout
        ///      Timeout before giving up if client is currently connecting to
        ///      server.  The call may block for up to this duration.
        /// @return
        ///      `true` if server is available, `false` otherwise.
        bool available(const dt::Duration &timeout = std::chrono::seconds(10)) const;

        /// @fn check
        /// @brief Check the provided gRPC status code, and throw an appropriate
        ///     exception if appropriate.
        ///
        /// @param[in] status
        ///     gRPC status code to check
        ///
        /// @throw shared::grpc::ServiceError
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

    //==========================================================================
    /// @class ClientWrapper<T>
    /// @brief Class template  for gRPC clients.
    ///
    /// Typical usage:
    ///
    ///  * Include "ClientWrapper" as a base for your stub class:
    ///    ```c++
    ///      #include "servicewrapper.h"
    ///      class YourClient : [...,] public ClientWrapper<CC::yourapp::YourService>
    ///      {
    ///      public:
    ///          YourClient (const std::string &identity,
    ///                      const std::string &serverAddress)
    ///              : ...,
    ///                ClientWrapper<CC::yourapp::YourService>(serverAddress),
    ///                ...
    ///              {}
    ///      }
    ///    ```
    ///
    ///  * Create client instance & launch
    ///
    ///      YourClient client("MyApplication", address);
    ///      ...
    ///
    ///  * Check the return status from a gRPC client stub; if not OK, raise an
    ///    appropritate exception:
    ///
    ///      void YourClient:yourmethod (...)
    ///      {
    ///          grpc::ClientContext cxt;
    ///          CC::yourapp::SomeRequest request = ...;
    ///          CC::yourapp::SomeResponse reply;
    ///          ...
    ///          grpc::Status status = this->stub->yourcall(&cxt, request, &reply);
    ///          ...
    ///      ->  this->check(status);  /// Raises exceptions if status != grpc::StatusCode::OK
    ///      }
    ///

    template <class T>
    class ClientWrapper : public ClientWrapperBase
    {
    public:
        using Stub = typename T::Stub;

    protected:
        template <class ResponseT, class RequestT>
        using gRPCMethod =
            ::grpc::Status (T::Stub::*)(
                ::grpc::ClientContext *,
                const RequestT &req,
                ResponseT *resp);

        template <class ResponseT, class RequestT>
        using gRPCReaderMethod =
            std::unique_ptr<::grpc::ClientReaderInterface<ResponseT>> (T::Stub::*)(
                ::grpc::ClientContext *,
                const RequestT &req);

    public:
        template <class... Args>
        ClientWrapper(const std::string &host,
                      Args &&...args)
            : ClientWrapperBase(T::service_full_name(),
                                host,
                                std::forward<Args>(args)...),
              stub(T::NewStub(this->channel))
        {
        }

        /// @fn call_sync
        /// @brief Direct invocation of a gRPC method
        /// @param[in] methodname
        ///     Method name, for debugging
        /// @param[in] method
        ///     gRPC stub method to inovke, in the form `&ClientClass::Stub::method`
        ///     (or with an suitable `using Stub = ...` statement, just `&Stub::method`).
        /// @param[in] request
        ///     ProtoBuf request message
        /// @param[out] response
        ///     Protobuf response message
        /// @param]in] wait_for_ready
        ///     Wait for gRPC service to become ready instead of failing.  The default
        ///     value can be changed via `ClientBase::set_wait_for_ready()`.
        /// @return
        ///      gRPC status code

        template <class ResponseT, class RequestT = google::protobuf::Empty>
        inline Status call_sync(const gRPCMethod<ResponseT, RequestT> &method,
                                const RequestT &request,
                                ResponseT *response,
                                std::optional<bool> wait_for_ready = {},
                                std::optional<dt::Duration> request_timeout = {}) const noexcept
        {
            ::grpc::ClientContext cxt;
            cxt.set_wait_for_ready(wait_for_ready.value_or(this->wait_for_ready));
            if (!request_timeout)
            {
                request_timeout = this->request_timeout;
            }

            if (request_timeout)
            {
                cxt.set_deadline(std::chrono::system_clock::now() + request_timeout.value());
            }
            return (this->stub.get()->*method)(&cxt, request, response);
        }

        template <class ResponseT, class RequestT = google::protobuf::Empty>
        inline Status call_sync(const std::string &methodname,
                                const gRPCMethod<ResponseT, RequestT> &method,
                                const RequestT &request,
                                ResponseT *response,
                                std::optional<bool> wait_for_ready = {},
                                std::optional<dt::Duration> request_timeout = {}) const noexcept
        {
            logf_trace("Invoking gRPC method %s(%s)", methodname, request);
            Status status = this->call_sync(method, request, response, wait_for_ready, request_timeout);
            if (status.ok())
            {
                logf_trace("Received gRPC method %s() response: %s", methodname, *response);
            }
            else
            {
                logf_debug("Received gRPC method %s() failure: %s", methodname, status);
            }
            return status;
        }

        template <class ResponseT, class RequestT = google::protobuf::Empty>
        inline Status call_sync(const std::string &methodname,
                                const gRPCMethod<ResponseT, RequestT> &method,
                                const RequestT &request,
                                std::optional<bool> wait_for_ready = {}) const noexcept
        {
            ResponseT response;
            return this->call_sync(methodname, method, request, &response, wait_for_ready);
        }

        /// @fn call_check
        /// @brief Direct invocation of a gRPC method, check for OK status code
        /// @param[in] methodname
        ///     Method name, for debugging
        /// @param[in] method
        ///     gRPC stub method to inovke, in the form `&ClientClass::Stub::method`
        ///     (or with an suitable `using Stub = ...` statement, just `&Stub::method`).
        /// @param[in] request
        ///     ProtoBuf request message
        /// @param]in] wait_for_ready
        ///     Wait for gRPC service to become ready instead of failing.
        /// @return
        ///     Protobuf response message
        /// @throw shared::grpc::ServiceError
        ///     Non-OK gRPC status code

        template <class ResponseT, class RequestT = google::protobuf::Empty>
        inline ResponseT call_check(const gRPCMethod<ResponseT, RequestT> &method,
                                    const RequestT &request = {},
                                    std::optional<bool> wait_for_ready = {},
                                    std::optional<dt::Duration> request_timeout = {}) const
        {
            ResponseT response;
            this->call_sync(method, request, &response, wait_for_ready, request_timeout).throw_if_error();
            return response;
        }

        template <class ResponseT, class RequestT = google::protobuf::Empty>
        inline ResponseT call_check(const std::string &methodname,
                                    const gRPCMethod<ResponseT, RequestT> &method,
                                    const RequestT &request = {},
                                    std::optional<bool> wait_for_ready = {}) const
        {
            ResponseT response;
            this->call_sync(methodname, method, request, &response, wait_for_ready).throw_if_error();
            return response;
        }

    public:
        std::unique_ptr<Stub> stub;
    };

}  // namespace shared::grpc
