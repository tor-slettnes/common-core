// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-requesthandler.h++
/// @brief Handle SysConfig gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/sysconfig/sysconfig.grpc.pb.h"  // Generated from `sysconfig.proto`
#include "sysconfig.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace sysconfig::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<::cc::platform::sysconfig::SysConfig>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<::cc::platform::sysconfig::SysConfig>;

    protected:
        //======================================================================
        // Product information

        ::grpc::Status GetProductInfo(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::ProductInfo* response) override;

        ::grpc::Status SetSerialNumber(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status SetModelName(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Host Information

        ::grpc::Status GetHostInfo(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::HostInfo* response) override;

        ::grpc::Status SetHostName(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Time configuration

        // Get or set time configuration

        ::grpc::Status SetTimeConfig(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeConfig* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetTimeConfig(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::TimeConfig* response) override;

        // Current timestamp
        ::grpc::Status SetCurrentTime(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Timestamp* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetCurrentTime(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Timestamp* response) override;

        //======================================================================
        // Time zone configuration

        ::grpc::Status ListTimezoneAreas(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::sysconfig::TimeZoneAreas* response) override;

        ::grpc::Status ListTimezoneCountries(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneArea* request,
            ::cc::platform::sysconfig::TimeZoneCountries* response) override;

        ::grpc::Status ListTimezoneRegions(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
            ::cc::platform::sysconfig::TimeZoneRegions* response) override;

        ::grpc::Status ListTimezoneSpecs(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneLocationFilter* request,
            ::cc::platform::sysconfig::TimeZoneCanonicalSpecs* response) override;

        ::grpc::Status GetTimezoneSpec(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneCanonicalName* request,
            ::cc::platform::sysconfig::TimeZoneCanonicalSpec* response) override;

        ::grpc::Status SetTimezone(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneConfig* request,
            ::cc::platform::sysconfig::TimeZoneInfo* response) override;

        ::grpc::Status GetTimezoneInfo(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::TimeZoneInfoRequest* request,
            ::cc::platform::sysconfig::TimeZoneInfo* response) override;

        //======================================================================
        // Spawn a new process, with or without capturing stdin/stdout/stderr.

        ::grpc::Status InvokeSync(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandInvocation* request,
            ::cc::platform::sysconfig::CommandResponse* response) override;

        ::grpc::Status InvokeAsync(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandInvocation* request,
            ::cc::platform::sysconfig::CommandInvocationResponse* response) override;

        ::grpc::Status InvokeFinish(
            ::grpc::ServerContext* context,
            const ::cc::platform::sysconfig::CommandContinuation* request,
            ::cc::platform::sysconfig::CommandResponse* response) override;

        ::grpc::Status Reboot(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        //======================================================================
        // Watch for changes from server

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const ::cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::sysconfig::Signal>* writer) override;
    };
}  // namespace sysconfig::grpc
