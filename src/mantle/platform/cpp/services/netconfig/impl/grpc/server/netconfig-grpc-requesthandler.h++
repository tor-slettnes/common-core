// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-requesthandler.h++
/// @brief Handle NetConfig gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "netconfig.grpc.pb.h"  // Generated from `netconfig.proto`
#include "netconfig.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::netconfig::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::platform::netconfig::NetConfig>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::platform::netconfig::NetConfig>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<netconfig::ProviderInterface>& provider);

        ::grpc::Status get_hostname(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::StringValue* response) override;

        ::grpc::Status set_hostname(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_connections(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::ConnectionMap* response) override;

        ::grpc::Status define_connection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::ConnectionRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status remove_connection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::MappingKey* request,
            ::google::protobuf::BoolValue* response) override;

        ::grpc::Status activate_connection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status deactivate_connection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_active_connections(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::ActiveConnectionMap* response) override;

        ::grpc::Status request_scan(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_aps(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::AccessPointMap* response) override;

        ::grpc::Status connect_ap(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::AccessPointConnection* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_devices(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::DeviceMap* response) override;

        ::grpc::Status get_global_data(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::GlobalData* response) override;

        ::grpc::Status set_wireless_enabled(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::RadioState* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status set_wireless_allowed(
            ::grpc::ServerContext* context,
            const ::google::protobuf::BoolValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status select_wireless_band(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::WirelessBandSetting* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::netconfig::Signal>* writer) override;

    private:
        std::shared_ptr<netconfig::ProviderInterface> provider;
    };
}  // namespace platform::netconfig::grpc
