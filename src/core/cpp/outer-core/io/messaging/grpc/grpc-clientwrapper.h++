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
#include "grpc-clientbase.h++"
#include "logging/logging.h++"
#include "chrono/date-time.h++"

#include <google/protobuf/empty.pb.h>
// #include <grpcpp/impl/codegen/sync_stream.h>
// #include <grpcpp/impl/codegen/client_context.h>

namespace core::grpc
{

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
    class ClientWrapper : public ClientBase
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

    public:
        template <class... Args>
        ClientWrapper(const std::string &host,
                      Args &&...args)
            : ClientBase(T::service_full_name(),
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
        /// @throw core::grpc::ServiceError
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

}  // namespace core::grpc
