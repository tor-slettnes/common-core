/// -*- c++ -*-
//==============================================================================
/// @file grpc-requesthandler.c++
/// @brief Server-side request handler gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-requesthandler.h++"
#include "protobuf-message.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "http-utils.h++"
//#include "errnos.h"

namespace core::grpc
{
    RequestHandlerBase::RequestHandlerBase(const std::string &full_service_name)
        : Base("gRPC Service", full_service_name)
    {
    }

    std::string RequestHandlerBase::address_setting() const
    {
        return this->realaddress(
            {},           // no provided address
            BIND_OPTION,  // host option
            PORT_OPTION,  // port option
            "[::]",       // default host
            8080);
    }

    Status RequestHandlerBase::failure(const std::exception &e,
                                       const std::string &operation,
                                       status::Flow flow,
                                       const fs::path &path,
                                       const int &lineno,
                                       const std::string &function)
    {
        Status status(*core::exception::map_to_event(e));
        this->log_status(status, operation, flow, path, lineno, function);
        return status;
    }

    Status RequestHandlerBase::failure(std::exception_ptr eptr,
                                       const std::string &operation,
                                       status::Flow flow,
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
            return this->failure(e, operation, flow, path, lineno, function);
        }
        catch (...)
        {
            Status status(::grpc::StatusCode::UNKNOWN,
                          "Unknown exception",
                          status::Domain::APPLICATION,
                          this->servicename());

            this->log_status(status, operation, flow, path, lineno, function);
            return status;
        }
    }

    Status RequestHandlerBase::failure(const std::exception &exception,
                                       const ::google::protobuf::Message &request,
                                       const std::string &peer,
                                       status::Flow flow,
                                       const fs::path &path,
                                       const int &lineno,
                                       const std::string &function)
    {
        return this->failure(exception,
                             this->request_description(request, peer, function),
                             flow,
                             path,
                             lineno,
                             function);
    }

    Status RequestHandlerBase::failure(std::exception_ptr eptr,
                                       const ::google::protobuf::Message &request,
                                       const std::string &peer,
                                       status::Flow flow,
                                       const fs::path &path,
                                       const int &lineno,
                                       const std::string &function)
    {
        return this->failure(eptr,
                             this->request_description(request, peer, function),
                             flow,
                             path,
                             lineno,
                             function);
    }

    void RequestHandlerBase::log_status(const Status &status,
                                        const std::string &operation,
                                        status::Flow flow,
                                        const fs::path &path,
                                        const int &lineno,
                                        const std::string &function)
    {
        auto msg = custom_log_msg(
            status::Level::NOTICE,
            log_scope,
            flow,
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

    std::string RequestHandlerBase::request_description(
        const ::google::protobuf::Message &request,
        const std::string &peer,
        const std::string &function)
    {
        std::stringstream ss;
        if (peer.size())
        {
            ss << "request from " << core::http::url_decode(peer) << ": ";
        }
        ss << function << "(" << core::protobuf::to_string(request) << ")";
        return ss.str();
    }

}  // namespace core::grpc
