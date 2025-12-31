// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-requesthandler.h++
/// @brief Handle NetConfig gRPC service requests
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/netconfig/grpc/netconfig_service.grpc.pb.h"
#include "netconfig.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace netconfig::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<::cc::platform::netconfig::grpc::NetConfig>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<::cc::platform::netconfig::grpc::NetConfig>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<netconfig::ProviderInterface>& provider);

        ::grpc::Status GetHostName(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::StringValue* response) override;

        ::grpc::Status SetHostName(
            ::grpc::ServerContext* context,
            const ::google::protobuf::StringValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetConnections(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::protobuf::ConnectionMap* response) override;

        ::grpc::Status DefineConnection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::ConnectionRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status RemoveConnection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::MappingKey* request,
            ::google::protobuf::BoolValue* response) override;

        ::grpc::Status ActivateConnection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status DeactivateConnection(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::MappingKey* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetActiveConnections(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::protobuf::ActiveConnectionMap* response) override;

        ::grpc::Status RequestScan(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetAccessPoints(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::protobuf::AccessPointMap* response) override;

        ::grpc::Status ConnectAccessPoint(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::WirelessConnectionRequest* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status GetDevices(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::protobuf::DeviceMap* response) override;

        ::grpc::Status GetGlobalData(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::netconfig::protobuf::GlobalData* response) override;

        ::grpc::Status SetWirelessEnabled(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::RadioState* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status SetWirelessAllowed(
            ::grpc::ServerContext* context,
            const ::google::protobuf::BoolValue* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status SelectWirelessBand(
            ::grpc::ServerContext* context,
            const ::cc::platform::netconfig::protobuf::WirelessBandSetting* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const ::cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::netconfig::protobuf::Signal>* writer) override;

    private:
        std::shared_ptr<netconfig::ProviderInterface> provider;
    };
}  // namespace netconfig::grpc
