// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-requesthandler.c++
/// @brief Handle SysConfig gRPC requests
/// @author Tor Slettnes
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

    ::grpc::Status RequestHandler::GetProductInfo(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::protobuf::ProductInfo* response)
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

    ::grpc::Status RequestHandler::SetSerialNumber(
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

    ::grpc::Status RequestHandler::SetModelName(
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

    ::grpc::Status RequestHandler::GetHostInfo(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::protobuf::HostInfo* response)
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

    ::grpc::Status RequestHandler::SetHostName(
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
    ::grpc::Status RequestHandler::SetTimeConfig(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeConfig* request,
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

    ::grpc::Status RequestHandler::GetTimeConfig(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::protobuf::TimeConfig* response)
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

    ::grpc::Status RequestHandler::SetCurrentTime(
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

    ::grpc::Status RequestHandler::GetCurrentTime(
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

    ::grpc::Status RequestHandler::ListTimezoneAreas(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneAreas* response)
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

    ::grpc::Status RequestHandler::ListTimezoneCountries(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneArea* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneCountries* response)
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

    ::grpc::Status RequestHandler::ListTimezoneRegions(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneLocationFilter* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneRegions* response)
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

    ::grpc::Status RequestHandler::ListTimezoneSpecs(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneLocationFilter* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalSpecs* response)
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
    ::grpc::Status RequestHandler::GetTimezoneSpec(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalName* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneCanonicalSpec* response)
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
    ::grpc::Status RequestHandler::SetTimezone(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneConfig* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneInfo* response)
    {
        try
        {
            TimeZoneInfo info;
            switch (request->config_case())
            {
            case ::cc::platform::sysconfig::protobuf::TimeZoneConfig::ConfigCase::kCanonicalZone:
                info = timezone->set_timezone(
                    request->canonical_zone());
                break;

            case ::cc::platform::sysconfig::protobuf::TimeZoneConfig::ConfigCase::kLocation:
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

    ::grpc::Status RequestHandler::GetTimezoneInfo(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::TimeZoneInfoRequest* request,
        ::cc::platform::sysconfig::protobuf::TimeZoneInfo* response)
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

    ::grpc::Status RequestHandler::InvokeSync(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::CommandInvocation* request,
        ::cc::platform::sysconfig::protobuf::CommandResponse* response)
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

    ::grpc::Status RequestHandler::InvokeAsync(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::CommandInvocation* request,
        ::cc::platform::sysconfig::protobuf::CommandInvocationResponse* response)
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

    ::grpc::Status RequestHandler::InvokeFinish(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::protobuf::CommandContinuation* request,
        ::cc::platform::sysconfig::protobuf::CommandResponse* response)
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

    ::grpc::Status RequestHandler::Reboot(
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

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext* context,
        const ::cc::protobuf::signal::Filter* filter,
        ::grpc::ServerWriter<::cc::platform::sysconfig::protobuf::Signal>* writer)
    {
        return this->stream_signals<::cc::platform::sysconfig::protobuf::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace sysconfig::grpc
