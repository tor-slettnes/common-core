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
#include "protobuf-version.h++"
#include "protobuf-inline.h++"
#include "platform/path.h++"

namespace cc::platform::sysconfig::grpc
{
    //======================================================================
    // Server status check

    ::grpc::Status RequestHandler::ServiceCheck(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ServiceCheckResponse* response)
    {
        response->set_api_level(APILEVEL_CURRENT);
        response->set_server_name(core::platform::path->exec_name());
        cc::protobuf::populate_version(response->mutable_server_version());
        return ::grpc::Status::OK;
    }


    //======================================================================
    // Product information

    ::grpc::Status RequestHandler::GetProductInfo(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::sysconfig::protobuf::ProductInfo* response)
    {
        try
        {
            cc::protobuf::encode(product->get_product_info(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    //======================================================================
    // Host Information

    ::grpc::Status RequestHandler::GetHostInfo(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::sysconfig::protobuf::HostInfo* response)
    {
        try
        {
            cc::protobuf::encode(host->get_host_info(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    //======================================================================
    // Time configuration

    // Get or set time configuration
    ::grpc::Status RequestHandler::SetTimeConfig(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeConfig* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            time->set_time_config(
                cc::protobuf::decoded<TimeConfig>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetTimeConfig(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::sysconfig::protobuf::TimeConfig* response)
    {
        try
        {
            cc::protobuf::encode(time->get_time_config(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
                cc::protobuf::decoded<core::dt::TimePoint>(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetCurrentTime(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::google::protobuf::Timestamp* response)
    {
        try
        {
            cc::protobuf::encode(time->get_current_time(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    //======================================================================
    // Time zone configuration
    //
    // Obtain geographic information about all available time zones.

    ::grpc::Status RequestHandler::ListTimezoneAreas(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        platform::sysconfig::protobuf::TimeZoneAreas* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                timezone->list_timezone_areas(),
                response->mutable_areas());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListTimezoneCountries(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneArea* request,
        platform::sysconfig::protobuf::TimeZoneCountries* response)
    {
        try
        {
            cc::protobuf::encode_vector(
                timezone->list_timezone_countries(request->name()),
                response->mutable_countries());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListTimezoneRegions(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneLocationFilter* request,
        platform::sysconfig::protobuf::TimeZoneRegions* response)
    {
        try
        {
            cc::protobuf::assign_repeated(
                sysconfig::timezone->list_timezone_regions(
                    cc::protobuf::decoded<TimeZoneLocationFilter>(*request)),
                response->mutable_regions());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ListTimezoneSpecs(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneLocationFilter* request,
        platform::sysconfig::protobuf::TimeZoneCanonicalSpecs* response)
    {
        try
        {
            cc::protobuf::encode_vector(
                sysconfig::timezone->list_timezone_specs(
                    cc::protobuf::decoded<TimeZoneLocationFilter>(*request)),
                response->mutable_specs());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    // Return geographic information about an arbitrary timezone.
    // If no zone name is provided, return information about the configured zone.
    ::grpc::Status RequestHandler::GetTimezoneSpec(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneCanonicalName* request,
        platform::sysconfig::protobuf::TimeZoneCanonicalSpec* response)
    {
        try
        {
            cc::protobuf::encode(
                sysconfig::timezone->get_timezone_spec(
                    cc::protobuf::decoded<TimeZoneCanonicalName>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    // Get or set the timezone configuration
    ::grpc::Status RequestHandler::SetTimezone(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneConfig* request,
        platform::sysconfig::protobuf::TimeZoneInfo* response)
    {
        try
        {
            TimeZoneInfo info;
            switch (request->config_case())
            {
            case platform::sysconfig::protobuf::TimeZoneConfig::ConfigCase::kCanonicalZone:
                info = timezone->set_timezone(
                    request->canonical_zone());
                break;

            case platform::sysconfig::protobuf::TimeZoneConfig::ConfigCase::kLocation:
                info = timezone->set_timezone(
                    cc::protobuf::decoded<TimeZoneLocation>(request->location()));
                break;

            default:
                break;
            }
            cc::protobuf::encode(info, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetTimezoneInfo(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::TimeZoneInfoRequest* request,
        platform::sysconfig::protobuf::TimeZoneInfo* response)
    {
        try
        {
            cc::protobuf::encode(
                timezone->get_timezone_info(
                    request->canonical_zone(),
                    cc::protobuf::decoded<core::dt::TimePoint>(request->time())),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    //======================================================================
    // Spawn a new process, with or without capturing stdin/stdout/stderr.

    ::grpc::Status RequestHandler::InvokeSync(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::CommandInvocation* request,
        platform::sysconfig::protobuf::CommandResponse* response)
    {
        try
        {
            InvocationResult result = process->invoke_sync(
                cc::protobuf::decoded<Invocation>(*request),
                request->stdin());

            cc::protobuf::encode(result, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::InvokeAsync(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::CommandInvocation* request,
        platform::sysconfig::protobuf::CommandInvocationResponse* response)
    {
        try
        {
            PID pid = process->invoke_async(
                cc::protobuf::decoded<Invocation>(*request),
                request->stdin());

            response->set_pid(pid);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::InvokeFinish(
        ::grpc::ServerContext* context,
        const platform::sysconfig::protobuf::CommandContinuation* request,
        platform::sysconfig::protobuf::CommandResponse* response)
    {
        try
        {
            InvocationResult result = process->invoke_finish(
                request->pid(),
                request->stdin());
            cc::protobuf::encode(result, response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    //======================================================================
    // Watch for changes from server

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext* context,
        const cc::protobuf::signal::Filter* filter,
        ::grpc::ServerWriter<platform::sysconfig::protobuf::Signal>* writer)
    {
        return this->stream_signals<platform::sysconfig::protobuf::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace cc::platform::sysconfig::grpc
