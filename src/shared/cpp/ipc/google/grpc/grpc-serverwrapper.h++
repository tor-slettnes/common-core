// -*- c++ -*-
//==============================================================================
/// @file grpc-serverwrapper.h++
/// @brief Server-side wrapper functionality for Common Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
///
/// Class templates for Common Core gRPC services (client and server), including:
///  * Settings store in YourServiceName.json, using JsonCpp as backend
///  * Status/error code wrappers
//==============================================================================

#pragma once
#include "grpc-basewrapper.h++"

#include <grpcpp/impl/codegen/sync_stream.h>

namespace cc::grpc
{
    //==========================================================================
    /// @class ServerWrapperBase
    /// @brief
    ///     Wapper for client-side gRPC invocations

    class ServerWrapperBase : public WrapperBase
    {
    protected:
        ServerWrapperBase(const std::string &fullServiceName,
                          const std::string &serviceAddress,
                          const std::shared_ptr<::grpc::ServerCredentials> &creds =
                              ::grpc::InsecureServerCredentials());

    public:
        /// @brief
        ///    Add this service to an existing gRPC ServerBuilder instance.
        ///
        /// @param[in,out] builder
        ///     gRPC Server Builder instance

        virtual void addToBuilder(::grpc::ServerBuilder *builder,
                                  bool addlistener = true);

        /// @brief
        ///     One-shot creation of a a new builder for this service, and
        ///     invocation of its corresponding BuildAndStart() method.  This is
        ///     mainly suitable for applications that host a single service; to
        ///     support multiple serices, see register().
        ///
        /// @return
        ///     The result of the ::grpc::ServerBuilder::BuildAndStart() invocation.
        std::unique_ptr<::grpc::Server> BuildAndStart();

        /// @brief
        ///     Convert an std::exception instance to a gRPC status code.
        /// @param[in] servicename
        ///     Service Name
        /// @param[in] eptr
        ///     Exception pointer
        /// @return
        ///     gRPC StatusCode object.
        ///
        /// @b Example
        /// Sample usage in a gRPC server:
        ///
        /// @code
        ///      try
        ///      {
        ///         handle_request(cxt, request, response);
        ///         return ::grpc::Status::OK;
        ///      }
        ///      catch (...)
        ///      {
        ///         return this->status(std::current_exception());
        ///      }
        /// @endcode

        // Status map_to_status(const std::exception &e);
        // Status map_to_status(const Event &event);

    protected:
        std::string request_description(const google::protobuf::Message &request,
                                        const std::string &peer,
                                        const std::string &function);

        void log_status(const Status &status,
                        const std::string &operation,
                        status::Flow flow = status::Flow::NONE,
                        const fs::path &path = __builtin_FILE(),
                        const int &line = __builtin_LINE(),
                        const std::string &function = __builtin_FUNCTION());

    protected:
        /// Log exception and return suitable gRPC status code.
        ///
        /// @param[in] servicename Service name (provided by derived templates)
        /// @param[in] exception exception instance
        /// @param[in] operation What failed, e.g. "methodname()".
        /// @return gRPC StatusCode object.

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

    public:
        std::string serviceAddress;
        std::shared_ptr<::grpc::ServerCredentials> credentials;
    };

    //==========================================================================
    /// @class ServerWrapper<T>
    /// @brief Class template for gRPC servers
    ///
    /// @b Examples
    ///  * Include a "ServerWrapper" as a base for your service class:
    ///    \code
    ///      #include "servicewrapper.h"
    ///      class YourService : public ServerWrapper<CC::yourapp::YourService>
    ///      {
    ///      public:
    ///          YourService (const std::string &interface)
    ///             : ServerWrapper<CC::yourapp::YourService>(interface) {}
    ///          ...
    ///      };
    ///    \endcode
    ///
    ///  * Create server instance & launch
    ///    \code
    ///      #include <grpc++/grpc.h>
    ///      int main (int argc, char **argv)
    ///      {
    ///        ...
    ///        ::grpc::ServerBuilder builder;
    ///        your::Server svc(address);
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
    class ServerWrapper : public ServerWrapperBase, public T::Service
    {
    public:
        using ServiceClass = T;

        template <class... Args>
        ServerWrapper(const std::string &serviceAddress = "",
                      const Args &...args)
            : ServerWrapperBase(TYPE_NAME_FULL(T), serviceAddress, std::forward<Args>(args)...)
        {
        }

        /// @brief
        ///    Add this service to an existing gRPC ServerBuilder instance.
        ///    Use this rather than BuildAndStart() when one builder hosts multiple services.
        ///
        /// @param[in,out] builder
        ///     gRPC Server Builder instance
        ///
        /// @param[in] addlistener
        ///     Add listening port to gRPC server. Set this to `false` if
        ///     listener is created separately prior to this invocation.
        ///
        /// @b Example
        /// @code
        ///    #include <grpc++/grpc++.h>
        ///
        ///    int main (int argc, char **argv)
        ///    {
        ///      ...
        ///      ::grpc::ServerBuilder builder;
        ///      your::Server service(...);
        ///      service.addToBuilder(&builder);
        ///      ...
        ///      auto server = builder.BuildAndStart();
        ///      ...
        ///      server->Wait();
        ///    }
        /// @endcode

        inline void addToBuilder(::grpc::ServerBuilder *builder,
                                 bool addlistener = true) override
        {
            if (addlistener)
            {
                ServerWrapperBase::addToBuilder(builder, addlistener);
            }
            builder->RegisterService(this);
        }

    public:
        // static inline Status map_to_status(const std::exception &e)
        // {
        //     return ServerWrapperBase::map_to_status(TYPE_NAME_BASE(T), e);
        // }

        /// Generate a Status instance suitable for reporting gRPC service status.
        static inline Status serviceStatus(
            ::grpc::StatusCode status_code,
            const std::string &text,
            status::Level level = status::Level::NONE,
            const types::KeyValueMap &attributes = {})
        {
            if (level == status::Level::NONE)
            {
                level = (status_code == ::grpc::StatusCode::OK)
                            ? status::Level::INFO
                            : status::Level::FAILED;
            }

            return Status(status_code,
                          text,
                          status::Domain::APPLICATION,
                          TYPE_NAME_BASE(T),
                          level,
                          dt::Clock::now(),
                          attributes);
        }
    };

}  // namespace cc::grpc
