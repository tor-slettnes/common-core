// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-requesthandler.h++
/// @brief Handle SysConfig gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig.grpc.pb.h"  // Generated from `sysconfig.proto`
#include "sysconfig.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::sysconfig::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::platform::sysconfig::SysConfig>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::platform::sysconfig::SysConfig>;

    protected:
        //======================================================================
        // Product information

        ::grpc::Status get_product_info(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::ProductInfo* response) override;

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
            ::cc::platform::sysconfig::HostInfo* response) override;

        ::grpc::Status set_host_name(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Time configuration

        // Get or set time configuration

        ::grpc::Status set_time_config(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeConfig* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_time_config(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::TimeConfig* response) override;

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

        ::grpc::Status list_timezone_areas(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::TimeZoneAreas* response) override;

        ::grpc::Status list_timezone_countries(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneArea* request,
            ::cc::platform::sysconfig::TimeZoneCountries* response) override;

        ::grpc::Status list_timezone_regions(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
            ::cc::platform::sysconfig::TimeZoneRegions* response) override;

        ::grpc::Status list_timezone_specs(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
            ::cc::platform::sysconfig::TimeZoneCanonicalSpecs* response) override;

        ::grpc::Status get_timezone_spec(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneCanonicalName* request,
            ::cc::platform::sysconfig::TimeZoneCanonicalSpec* response) override;

        ::grpc::Status set_timezone(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneConfig* request,
            ::cc::platform::sysconfig::TimeZoneInfo* response) override;

        ::grpc::Status get_timezone_info(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneInfoRequest* request,
            ::cc::platform::sysconfig::TimeZoneInfo* response) override;

        //======================================================================
        // Spawn a new process, with or without capturing stdin/stdout/stderr.

        ::grpc::Status invoke_sync(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandInvocation* request,
            ::cc::platform::sysconfig::CommandResponse* response) override;

        ::grpc::Status invoke_async(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandInvocation* request,
            ::cc::platform::sysconfig::CommandInvocationResponse* response) override;

        ::grpc::Status invoke_finish(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandContinuation* request,
            ::cc::platform::sysconfig::CommandResponse* response) override;

        ::grpc::Status reboot(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Watch for changes from server

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::sysconfig::Signal>* writer) override;
    };
}  // namespace platform::sysconfig::grpc
