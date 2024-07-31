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

namespace platform::sysconfig::grpc
{
    //======================================================================
    // Product information

    ::grpc::Status RequestHandler::get_product_info(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::ProductInfo* response)
    {
        try
        {
            protobuf::encode(platform::sysconfig::product->get_product_info(), response);
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
            platform::sysconfig::product->set_serial_number(request->value());
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
            platform::sysconfig::product->set_model_name(request->value());
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
        ::cc::platform::sysconfig::HostInfo* response)
    {
        try
        {
            protobuf::encode(platform::sysconfig::hostconfig->get_host_info(), response);
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
            platform::sysconfig::hostconfig->set_host_name(request->value());
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
        const ::cc::platform::sysconfig::TimeConfig* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            platform::sysconfig::time->set_time_config(
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
        ::cc::platform::sysconfig::TimeConfig* response)
    {
        try
        {
            protobuf::encode(platform::sysconfig::time->get_time_config(), response);
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
            platform::sysconfig::time->set_current_time(
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
            protobuf::encode(platform::sysconfig::time->get_current_time(), response);
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
        ::cc::platform::sysconfig::TimeZoneAreas* response)
    {
        try
        {
            protobuf::assign_repeated(
                platform::sysconfig::timezone->list_timezone_areas(),
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
        const ::cc::platform::sysconfig::TimeZoneArea* request,
        ::cc::platform::sysconfig::TimeZoneCountries* response)
    {
        try
        {
            protobuf::encode_vector(
                platform::sysconfig::timezone->list_timezone_countries(request->name()),
                response->mutable_countries());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::list_timezone_specs(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
        ::cc::platform::sysconfig::TimeZoneCanonicalSpecs* response)
    {
        try
        {
            protobuf::encode_vector(
                platform::sysconfig::timezone->list_timezone_specs(
                    protobuf::decoded<platform::sysconfig::TimeZoneLocationFilter>(*request)),
                response->mutable_specs());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::read_timezone_specs(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
        ::grpc::ServerWriter<::cc::platform::sysconfig::TimeZoneCanonicalSpec>* writer)
    {
        ::cc::platform::sysconfig::TimeZoneCanonicalSpecs response;
        this->list_timezone_specs(context, request, &response);

        try
        {
            for (const ::cc::platform::sysconfig::TimeZoneCanonicalSpec& spec : response.specs())
            {
                writer->Write(spec);
            }
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
        const ::cc::platform::sysconfig::TimeZoneCanonicalName* request,
        ::cc::platform::sysconfig::TimeZoneCanonicalSpec* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::timezone->get_timezone_spec(request->name()),
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
        const ::cc::platform::sysconfig::TimeZoneConfig* request,
        ::cc::platform::sysconfig::TimeZoneInfo* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::timezone->set_timezone(
                    protobuf::decoded<TimeZoneConfig>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_timezone_config(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::platform::sysconfig::TimeZoneConfig* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::timezone->get_timezone_config(),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_timezone_info(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::TimeZoneInfoRequest* request,
        ::cc::platform::sysconfig::TimeZoneInfo* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::timezone->get_timezone_info(
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
        const ::cc::platform::sysconfig::CommandInvocation* request,
        ::cc::platform::sysconfig::CommandResponse* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::process->invoke_sync(
                    protobuf::decoded<CommandInvocation>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_async(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::CommandInvocation* request,
        ::cc::platform::sysconfig::CommandInvocationResponse* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::process->invoke_async(
                    protobuf::decoded<CommandInvocation>(*request)),
                response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_finish(
        ::grpc::ServerContext* context,
        const ::cc::platform::sysconfig::CommandContinuation* request,
        ::cc::platform::sysconfig::CommandResponse* response)
    {
        try
        {
            protobuf::encode(
                platform::sysconfig::process->invoke_finish(
                    protobuf::decoded<CommandContinuation>(*request)),
                response);
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
            platform::sysconfig::hostconfig->reboot();
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
        ::grpc::ServerWriter<::cc::platform::sysconfig::Signal>* writer)
    {
        return this->stream_signals<cc::platform::sysconfig::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace platform::sysconfig::grpc
