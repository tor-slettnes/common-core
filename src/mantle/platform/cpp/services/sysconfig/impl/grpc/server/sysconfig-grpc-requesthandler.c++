// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-requesthandler.c++
/// @brief Handle SysConfig gRPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-requesthandler.h++"
#include "sysconfig-grpc-signalqueue.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"

namespace sysconfig::grpc
{
    //======================================================================
    // Product information

    ::grpc::Status RequestHandler::get_product_info(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::sysconfig::ProductInfo* response)
    {
        try
        {
            protobuf::encode(product->get_product_info(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_serial_number(
        ::grpc::ServerContext* context,
        const ::google::protobuf::StringValue* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            product->set_serial_number(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_model_name(
        ::grpc::ServerContext* context,
        const ::google::protobuf::StringValue* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            product->set_model_name(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    //======================================================================
    // Host Information

    ::grpc::Status RequestHandler::get_host_info(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::sysconfig::HostInfo* response)
    {
        try
        {
            protobuf::encode(host->get_host_info(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::set_host_name(
        ::grpc::ServerContext* context,
        const ::google::protobuf::StringValue* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            host->set_host_name(request->value());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    //======================================================================
    // Time configuration

    // Get or set time configuration
    ::grpc::Status RequestHandler::set_time_config(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeConfig* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            time->set_time_config(
                protobuf::decoded<TimeConfig>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_time_config(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::sysconfig::TimeConfig* response)
    {
        try
        {
            protobuf::encode(time->get_time_config(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    // Current timestamp

    ::grpc::Status RequestHandler::set_current_time(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Timestamp* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            time->set_current_time(
                protobuf::decoded<core::dt::TimePoint>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_current_time(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Timestamp* response)
    {
        try
        {
            protobuf::encode(time->get_current_time(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    //======================================================================
    // Time zone configuration
    //
    // Obtain geographic information about all available time zones.

    ::grpc::Status RequestHandler::list_timezone_areas(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::sysconfig::TimeZoneAreas* response)
    {
        try
        {
            protobuf::assign_repeated(
                timezone->list_timezone_areas(),
                response->mutable_areas());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_timezone_countries(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneArea* request,
        ::cc::sysconfig::TimeZoneCountries* response)
    {
        try
        {
            protobuf::encode_vector(
                timezone->list_timezone_countries(request->name()),
                response->mutable_countries());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_timezone_regions(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneLocationFilter* request,
        ::cc::sysconfig::TimeZoneRegions* response)
    {
        try
        {
            protobuf::assign_repeated(
                sysconfig::timezone->list_timezone_regions(
                    protobuf::decoded<TimeZoneLocationFilter>(*request)),
                response->mutable_regions());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_timezone_specs(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneLocationFilter* request,
        ::cc::sysconfig::TimeZoneCanonicalSpecs* response)
    {
        try
        {
            protobuf::encode_vector(
                sysconfig::timezone->list_timezone_specs(
                    protobuf::decoded<TimeZoneLocationFilter>(*request)),
                response->mutable_specs());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    // Return geographic information about an arbitrary timezone.
    // If no zone name is provided, return information about the configured zone.
    ::grpc::Status RequestHandler::get_timezone_spec(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneCanonicalName* request,
        ::cc::sysconfig::TimeZoneCanonicalSpec* response)
    {
        try
        {
            protobuf::encode(
                sysconfig::timezone->get_timezone_spec(
                    protobuf::decoded<TimeZoneCanonicalName>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    // Get or set the timezone configuration
    ::grpc::Status RequestHandler::set_timezone(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneConfig* request,
        ::cc::sysconfig::TimeZoneInfo* response)
    {
        try
        {
            TimeZoneInfo info;
            switch (request->config_case())
            {
            case ::cc::sysconfig::TimeZoneConfig::ConfigCase::kCanonicalZone:
                info = timezone->set_timezone(
                    request->canonical_zone());
                break;

            case ::cc::sysconfig::TimeZoneConfig::ConfigCase::kLocation:
                info = timezone->set_timezone(
                    protobuf::decoded<TimeZoneLocation>(request->location()));
                break;

            default:
                break;
            }
            protobuf::encode(info, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_timezone_info(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::TimeZoneInfoRequest* request,
        ::cc::sysconfig::TimeZoneInfo* response)
    {
        try
        {
            protobuf::encode(
                timezone->get_timezone_info(
                    request->canonical_zone(),
                    protobuf::decoded<core::dt::TimePoint>(request->time())),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    //======================================================================
    // Spawn a new process, with or without capturing stdin/stdout/stderr.

    ::grpc::Status RequestHandler::invoke_sync(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::CommandInvocation* request,
        ::cc::sysconfig::CommandResponse* response)
    {
        try
        {
            InvocationResult result = process->invoke_sync(
                protobuf::decoded<Invocation>(*request),
                request->stdin());

            protobuf::encode(result, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_async(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::CommandInvocation* request,
        ::cc::sysconfig::CommandInvocationResponse* response)
    {
        try
        {
            PID pid = process->invoke_async(
                protobuf::decoded<Invocation>(*request),
                request->stdin());

            response->set_pid(pid);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_finish(
        ::grpc::ServerContext* context,
        const ::cc::sysconfig::CommandContinuation* request,
        ::cc::sysconfig::CommandResponse* response)
    {
        try
        {
            InvocationResult result = process->invoke_finish(
                request->pid(),
                request->stdin());
            protobuf::encode(result, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::reboot(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            host->reboot();
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    //======================================================================
    // Watch for changes from server

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext* context,
        const ::cc::signal::Filter* filter,
        ::grpc::ServerWriter<::cc::sysconfig::Signal>* writer)
    {
        return this->stream_signals<::cc::sysconfig::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace sysconfig::grpc
