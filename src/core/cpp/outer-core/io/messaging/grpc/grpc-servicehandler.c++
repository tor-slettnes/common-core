/// -*- c++ -*-
//==============================================================================
/// @file grpc-serverwrapper.c++
/// @brief Server-side wrapper functionality for Comon Core gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-servicehandler.h++"
#include "protobuf-message.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "http-utils.h++"
//#include "errnos.h"


namespace shared::grpc
{
    ServiceHandlerBase::ServiceHandlerBase(const std::string &full_service_name)
        : Base("gRPC Service", full_service_name)
    {
    }

    std::string ServiceHandlerBase::address_setting() const
    {
        return this->realaddress(
            {},           // no provided address
            BIND_OPTION,  // host option
            PORT_OPTION,  // port option
            "[::]",       // default host
            8080);
    }

    Status ServiceHandlerBase::failure(const std::exception &e,
                                       const std::string &operation,
                                       status::Flow flow,
                                       const std::filesystem::path &path,
                                       const int &lineno,
                                       const std::string &function)
    {
        Status status(*shared::exception::map_to_event(e));
        this->log_status(status, operation, flow, path, lineno, function);
        return status;
    }

    Status ServiceHandlerBase::failure(std::exception_ptr eptr,
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

    Status ServiceHandlerBase::failure(const std::exception &exception,
                                       const google::protobuf::Message &request,
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

    Status ServiceHandlerBase::failure(std::exception_ptr eptr,
                                       const google::protobuf::Message &request,
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

    void ServiceHandlerBase::log_status(const Status &status,
                                        const std::string &operation,
                                        status::Flow flow,
                                        const fs::path &path,
                                        const int &lineno,
                                        const std::string &function)
    {
        auto msg = custom_log_msg(
            status::Level::NOTICE,
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

    std::string ServiceHandlerBase::request_description(
        const google::protobuf::Message &request,
        const std::string &peer,
        const std::string &function)
    {
        std::stringstream ss;
        if (peer.size())
        {
            ss << "request from " << shared::http::url_decode(peer) << ": ";
        }
        ss << function << "(" << ::protobuf::to_string(request) << ")";
        return ss.str();
    }

}  // namespace shared::grpc
