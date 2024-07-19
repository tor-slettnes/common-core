// -*- c++ -*-
//==============================================================================
/// @file grpc-requesthandler.h++
/// @brief Server-side request handler gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-base.h++"
#include "protobuf-standard-types.h++"

namespace core::grpc
{
    //==========================================================================
    /// @class RequestHandlerBase
    /// @brief
    ///     Wapper for client-side gRPC invocations

    class RequestHandlerBase : public Base
    {
    protected:
        RequestHandlerBase(const std::string &full_service_name);

    public:
        std::string address_setting() const;

    protected:
        Status failure(const std::exception &exception,
                       const std::string &operation,
                       status::Flow flow = status::Flow::ABORTED,
                       const fs::path &path = __builtin_FILE(),
                       const int &line = __builtin_LINE(),
                       const std::string &function = __builtin_FUNCTION());

        Status failure(std::exception_ptr eptr,
                       const std::string &operation,
                       status::Flow flow = status::Flow::ABORTED,
                       const fs::path &path = __builtin_FILE(),
                       const int &line = __builtin_LINE(),
                       const std::string &function = __builtin_FUNCTION());

        Status failure(const std::exception &exception,
                       const ::google::protobuf::Message &request,
                       const std::string &peer = {},
                       status::Flow flow = status::Flow::ABORTED,
                       const fs::path &path = __builtin_FILE(),
                       const int &line = __builtin_LINE(),
                       const std::string &function = __builtin_FUNCTION());

        Status failure(std::exception_ptr eptr,
                       const ::google::protobuf::Message &request,
                       const std::string &peer = {},
                       status::Flow flow = status::Flow::ABORTED,
                       const fs::path &path = __builtin_FILE(),
                       const int &line = __builtin_LINE(),
                       const std::string &function = __builtin_FUNCTION());

        void log_status(const Status &status,
                        const std::string &operation,
                        status::Flow flow = status::Flow::NONE,
                        const fs::path &path = __builtin_FILE(),
                        const int &line = __builtin_LINE(),
                        const std::string &function = __builtin_FUNCTION());

        std::string request_description(const ::google::protobuf::Message &request,
                                        const std::string &peer,
                                        const std::string &function);
    };

    //==========================================================================
    /// @class RequestHandler<T>
    /// @brief Class template for gRPC services
    ///
    /// @b Examples
    ///  * Include a "RequestHandler" as a base for your service class:
    ///    \code
    ///      #include "servicewrapper.h"
    ///      class YourService : public RequestHandler<core::yourapp::YourService>
    ///      {
    ///      public:
    ///          YourService (const std::string &interface)
    ///             : RequestHandler<core::yourapp::YourService>(interface) {}
    ///          ...
    ///      };
    ///    \endcode
    ///
    ///  * Create service instance & launch
    ///    \code
    ///      #include <grpc++/grpc.h>
    ///      int main (int argc, char **argv)
    ///      {
    ///        ...
    ///        core::grpc::ServerBuilder builder;
    ///        builder.add_service(std::make_shared<MyRequestHandler>());
    ///        ...
    ///        auto server = builder.BuildAndStart();
    ///        ...
    ///        server->Wait();
    ///    \endcode
    ///
    ///  * Catch internal exceptions and return appropriate gRPC status
    ///    \code
    ///      ::grpc::Status YourService:yourmethod (::grpc::ServerContext* cxt,
    ///                                           const core::yourapp::SomeRequest *request,
    ///                                           core::yourapp::SomeResponse *reply)
    ///      {
    ///        try
    ///        {
    ///          handle(request);
    ///        }
    ///        catch (std::exception &e)
    ///        {
    ///          return this->failure(e, *request, cxt->peer());
    ///        }
    ///      }
    ///    \endcode

    template <class T>
    class RequestHandler : public RequestHandlerBase, public T::Service
    {
    public:
        using ServiceClass = T;

        RequestHandler()
            : RequestHandlerBase(T::service_full_name())
        {
        }

        template <class ResponseType = ::google::protobuf::Empty,
                  class RequestType = ::google::protobuf::Empty>
        ::grpc::Status wrap(::grpc::ServerContext *context,
                            const RequestType *request,
                            ResponseType *response,
                            const std::function<ResponseType(const RequestType &)> &function,
                            status::Flow flow = status::Flow::ABORTED,
                            const fs::path &src_path = __builtin_FILE(),
                            const int &src_line = __builtin_LINE(),
                            const std::string &src_function = __builtin_FUNCTION())
        {
            try
            {
                *response = function(*request);
                return ::grpc::Status::OK;
            }
            catch (...)
            {
                return this->failure(std::current_exception(),
                                     *request,
                                     context->peer(),
                                     flow,
                                     src_path,
                                     src_line,
                                     src_function);
            }
        }
    };

}  // namespace core::grpc
