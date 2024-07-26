// -*- c++ -*-
//==============================================================================
/// @file network-grpc-requesthandler.h++
/// @brief Handle Network gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "network.grpc.pb.h"  // Generated from `network.proto`
#include "network-provider.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::network::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::network::Network>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::RequestHandler<cc::network::Network>;

    protected:
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
            ::cc::network::ConnectionMap* response) override;

        ::grpc::Status define_connection(
            ::grpc::ServerContext* context,
            const ::cc::network::ConnectionRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status remove_connection(
            ::grpc::ServerContext* context,
            const ::cc::network::MappingKey* request,
            ::google::protobuf::BoolValue* response) override;

        ::grpc::Status activate_connection(
            ::grpc::ServerContext* context,
            const ::cc::network::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status deactivate_connection(
            ::grpc::ServerContext* context,
            const ::cc::network::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_active_connections(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::network::ActiveConnectionMap* response) override;

        ::grpc::Status request_scan(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_aps(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::network::AccessPointMap* response) override;

        ::grpc::Status connect_ap(
            ::grpc::ServerContext* context,
            const ::cc::network::AccessPointConnection* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status get_devices(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::network::DeviceMap* response) override;

        ::grpc::Status get_global_data(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::network::GlobalData* response) override;

        ::grpc::Status set_wireless_enabled(
            ::grpc::ServerContext* context,
            const ::cc::network::RadioState* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status set_wireless_allowed(
            ::grpc::ServerContext* context,
            const ::google::protobuf::BoolValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status select_wireless_band(
            ::grpc::ServerContext* context,
            const ::cc::network::WirelessBandSetting* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::network::Signal>* writer) override;
    };
}  // namespace platform::network::grpc
