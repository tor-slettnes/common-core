// -*- c++ -*-
//==============================================================================
/// @file system-grpc-requesthandler.c++
/// @brief Handle System RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-grpc-requesthandler.h++"
#include "system-grpc-signalqueue.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-system-types.h++"
#include "protobuf-inline.h++"

namespace platform::system::grpc
{
    //======================================================================
    // Product information

    ::grpc::Status RequestHandler::get_product_info(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::system::ProductInfo* response)
    {
        try
        {
            response->CopyFrom(platform::system::product->get_product_info());
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
            platform::system::product->set_serial_number(request->value());
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
            platform::system::product->set_model_name(request->value());
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
        ::cc::system::HostInfo* response)
    {
        try
        {
            response->CopyFrom(platform::system::hostconfig->get_host_info());
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
            platform::system::hostconfig->set_host_name(request->value());
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
        const ::cc::system::TimeConfig* request,
        ::google::protobuf::Empty* response)
    {
        try
        {
            platform::system::time->set_time_config(*request);
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
        ::cc::system::TimeConfig* response)
    {
        try
        {
            response->CopyFrom(platform::system::time->get_time_config());
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
            platform::system::time->set_current_time(
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
            protobuf::encode(platform::system::time->get_current_time(), response);
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

    ::grpc::Status RequestHandler::get_timezone_specs(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::grpc::ServerWriter<::cc::system::TimeZoneSpec>* writer)
    {
        try
        {
            for (const TimeZoneSpec& spec : platform::system::timezone->get_timezone_specs())
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
        const ::cc::system::TimeZoneName* request,
        ::cc::system::TimeZoneSpec* response)
    {
        try
        {
            response->CopyFrom(
                platform::system::timezone->get_timezone_spec(
                    request->zonename()));
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
        const ::cc::system::TimeZoneConfig* request,
        ::cc::system::TimeZoneInfo* response)
    {
        try
        {
            platform::system::timezone->set_timezone(*request);
            protobuf::encode(platform::system::timezone->get_current_timezone(), response);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_configured_timezone(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::system::TimeZoneConfig* response)
    {
        try
        {
            response->CopyFrom(platform::system::timezone->get_configured_timezone());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::get_current_timezone(
        ::grpc::ServerContext* context,
        const ::google::protobuf::Empty* request,
        ::cc::system::TimeZoneInfo* response)
    {
        try
        {
            protobuf::encode(platform::system::timezone->get_current_timezone(), response);
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
        const ::cc::system::CommandInvocation* request,
        ::cc::system::CommandResponse* response)
    {
        try
        {
            response->CopyFrom(platform::system::process->invoke_sync(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_async(
        ::grpc::ServerContext* context,
        const ::cc::system::CommandInvocation* request,
        ::cc::system::CommandInvocationStatus* response)
    {
        try
        {
            response->CopyFrom(platform::system::process->invoke_async(*request));
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::invoke_finish(
        ::grpc::ServerContext* context,
        const ::cc::system::CommandInput* request,
        ::cc::system::CommandResponse* response)
    {
        try
        {
            response->CopyFrom(platform::system::process->invoke_finish(*request));
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
            platform::system::hostconfig->reboot();
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
        ::grpc::ServerWriter<::cc::system::Signal>* writer)
    {
        return this->stream_signals<cc::system::Signal, SignalQueue>(
            context,
            filter,
            writer);
    }
}  // namespace platform::system::grpc
