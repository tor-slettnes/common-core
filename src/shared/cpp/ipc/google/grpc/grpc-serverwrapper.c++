/// -*- c++ -*-
//==============================================================================
/// @file grpc-serverwrapper.c++
/// @brief Server-side wrapper functionality for Comon Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-serverwrapper.h++"
#include "protobuf-message.h++"
//#include "errnos.h"

namespace cc::grpc
{
    ServerWrapperBase::ServerWrapperBase(
        const std::string &fullServiceName,
        const std::string &serviceAddress,
        const std::shared_ptr<::grpc::ServerCredentials> &creds)
        : WrapperBase(fullServiceName),
          serviceAddress(this->realaddress(serviceAddress,
                                           BIND_OPTION,
                                           PORT_OPTION,
                                           "[::]")),
          credentials(creds)
    {
    }

    void ServerWrapperBase::addToBuilder(::grpc::ServerBuilder *builder,
                                         bool addlistener)
    {
        if (addlistener)
        {
            builder->AddListeningPort(this->serviceAddress, this->credentials);
            logf_debug(
                "Registered listener on %s for %s",
                this->serviceAddress,
                this->servicename());
        }
    }

    std::unique_ptr<::grpc::Server> ServerWrapperBase::BuildAndStart()
    {
        ::grpc::ServerBuilder builder;
        this->addToBuilder(&builder);
        return builder.BuildAndStart();
    }

    std::string ServerWrapperBase::request_description(
        const google::protobuf::Message &request,
        const std::string &peer,
        const std::string &function)
    {
        if (peer.size())
        {
            return str::format("request from %s: %s(%s)",
                               peer,
                               function,
                               cc::protobuf::to_string(request));
        }
        else
        {
            return str::format("%s(%s)", function, request);
        }
    }

    void ServerWrapperBase::log_status(const Status &status,
                                       const std::string &operation,
                                       const fs::path &path,
                                       const int &lineno,
                                       const std::string &function)
    {
        auto msg = custom_log_message(
            status::Level::NOTICE,
            dt::Clock::now(),
            path,
            lineno,
            function);

        if (status.error_code() == ::grpc::StatusCode::CANCELLED)
        {
            msg->format("Canceled %s", operation);
        }
        else
        {
            msg->format("Failed %s: %s", operation, status);
        }
        msg->dispatch();
    }

    Status ServerWrapperBase::failure(const std::exception &e,
                                      const std::string &operation,
                                      const std::filesystem::path &path,
                                      const int &lineno,
                                      const std::string &function)
    {
        Status status(*cc::exception::map_to_error(e));
        this->log_status(status, operation, path, lineno, function);
        return status;
    }

    Status ServerWrapperBase::failure(std::exception_ptr eptr,
                                      const std::string &operation,
                                      const fs::path &path,
                                      const int &lineno,
                                      const std::string &function)
    {
        try
        {
            std::rethrow_exception(eptr);
        }
        catch (const std::exception &e)
        {
            return this->failure(e, operation, path, lineno, function);
        }
        catch (...)
        {
            Status status(::grpc::StatusCode::UNKNOWN,
                          "Unknown exception",
                          status::Domain::APPLICATION,
                          this->servicename());

            this->log_status(status, operation, path, lineno, function);
            return status;
        }
    }

    Status ServerWrapperBase::failure(const std::exception &exception,
                                      const google::protobuf::Message &request,
                                      const std::string &peer,
                                      const fs::path &path,
                                      const int &lineno,
                                      const std::string &function)
    {
        return this->failure(exception,
                             this->request_description(request, peer, function),
                             path,
                             lineno,
                             function);
    }

    Status ServerWrapperBase::failure(std::exception_ptr eptr,
                                      const google::protobuf::Message &request,
                                      const std::string &peer,
                                      const fs::path &path,
                                      const int &lineno,
                                      const std::string &function)
    {
        return this->failure(eptr,
                             this->request_description(request, peer, function),
                             path,
                             lineno,
                             function);
    }

}  // namespace cc::grpc
