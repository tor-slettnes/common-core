// -*- c++ -*-
//==============================================================================
/// @file system-grpc-requesthandler.h++
/// @brief Handle System gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system.grpc.pb.h"  // Generated from `system.proto`
#include "system-providers.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::system::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::system::System>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::system::System>;

    protected:
        //======================================================================
        // Product information

        ::grpc::Status get_product_info(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::system::ProductInfo* response) override;

        ::grpc::Status set_serial_number(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status set_model_name(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Host Information

        ::grpc::Status get_host_info(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::system::HostInfo* response) override;

        ::grpc::Status set_host_name(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Time configuration

        // Get or set time configuration

        ::grpc::Status set_time_config(
            ::grpc::ServerContext* context,
            const ::cc::system::TimeConfig* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_time_config(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::system::TimeConfig* response) override;

        // Current timestamp
        ::grpc::Status set_current_time(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Timestamp* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_current_time(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Timestamp* response) override;

        //======================================================================
        // Time zone configuration
        //
        // Obtain geographic information about all available time zones.

        ::grpc::Status get_timezone_specs(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::grpc::ServerWriter<::cc::system::TimeZoneSpec>* writer) override;

        // Return geographic information about an arbitrary timezone.
        // If no zone name is provided, return information about the configured zone.
        ::grpc::Status get_timezone_spec(
            ::grpc::ServerContext* context,
            const ::cc::system::TimeZoneName* request,
            ::cc::system::TimeZoneSpec* response) override;

        // Get or set the timezone configuration
        ::grpc::Status set_timezone(
            ::grpc::ServerContext* context,
            const ::cc::system::TimeZoneConfig* request,
            ::cc::system::TimeZoneInfo* response) override;

        ::grpc::Status get_configured_timezone(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::system::TimeZoneConfig* response) override;

        ::grpc::Status get_current_timezone(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::system::TimeZoneInfo* response) override;

        //======================================================================
        // Spawn a new process, with or without capturing stdin/stdout/stderr.

        ::grpc::Status invoke_sync(
            ::grpc::ServerContext* context,
            const ::cc::system::CommandInvocation* request,
            ::cc::system::CommandResponse* response) override;

        ::grpc::Status invoke_async(
            ::grpc::ServerContext* context,
            const ::cc::system::CommandInvocation* request,
            ::cc::system::CommandInvocationStatus* response) override;

        ::grpc::Status invoke_finish(
            ::grpc::ServerContext* context,
            const ::cc::system::CommandInput* request,
            ::cc::system::CommandResponse* response) override;

        ::grpc::Status reboot(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Watch for changes from server

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::system::Signal>* writer) override;
    };
}  // namespace platform::system::grpc
