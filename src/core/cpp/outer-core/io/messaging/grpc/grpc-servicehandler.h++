// -*- c++ -*-
//==============================================================================
/// @file grpc-servicewrapper.h++
/// @brief Service-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-base.h++"

#include <google/protobuf/empty.pb.h>

namespace core::grpc
{
    //==========================================================================
    /// @class ServiceHandlerBase
    /// @brief
    ///     Wapper for client-side gRPC invocations

    class ServiceHandlerBase : public Base
    {
    protected:
        ServiceHandlerBase(const std::string &full_service_name);

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
                       const google::protobuf::Message &request,
                       const std::string &peer = {},
                       status::Flow flow = status::Flow::ABORTED,
                       const fs::path &path = __builtin_FILE(),
                       const int &line = __builtin_LINE(),
                       const std::string &function = __builtin_FUNCTION());

        Status failure(std::exception_ptr eptr,
                       const google::protobuf::Message &request,
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

        std::string request_description(const google::protobuf::Message &request,
                                        const std::string &peer,
                                        const std::string &function);
    };

    //==========================================================================
    /// @class ServiceHandler<T>
    /// @brief Class template for gRPC services
    ///
    /// @b Examples
    ///  * Include a "ServiceHandler" as a base for your service class:
    ///    \code
    ///      #include "servicewrapper.h"
    ///      class YourService : public ServiceHandler<CC::yourapp::YourService>
    ///      {
    ///      public:
    ///          YourService (const std::string &interface)
    ///             : ServiceHandler<CC::yourapp::YourService>(interface) {}
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
    ///        ::grpc::ServerBuilder builder;
    ///        your::Service svc(address);
    ///        builder.RegisterService(&svc);
    ///        svc.addToBuilder(&builder);
    ///        ...
    ///        auto server = builder.BuildAndStart();
    ///        ...
    ///        server->Wait();
    ///    \endcode
    ///
    ///  * Catch internal exceptions and return appropriate gRPC status
    ///    \code
    ///      ::grpc::Status YourService:yourmethod (::grpc::ServerContext* cxt,
    ///                                           const CC::yourapp::SomeRequest *request,
    ///                                           CC::yourapp::SomeResponse *reply)
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
    ///    \endcode;

    template <class T>
    class ServiceHandler : public ServiceHandlerBase, public T::Service
    {
    public:
        using ServiceClass = T;

        ServiceHandler()
            : ServiceHandlerBase(T::service_full_name())
        {
        }

        template <class ResponseType = google::protobuf::Empty,
                  class RequestType = google::protobuf::Empty>
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
