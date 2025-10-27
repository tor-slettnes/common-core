// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-requesthandler.h++
/// @brief Handle Upgrade gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/upgrade/grpc/upgrade.grpc.pb.h"  // Generated from `upgrade.proto`
#include "upgrade-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace upgrade::grpc
{
    /// Process requests from Upgrade gRPC clients
    class RequestHandler
        : public core::grpc::SignalRequestHandler<::cc::platform::upgrade::grpc::Upgrade>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<::cc::platform::upgrade::grpc::Upgrade>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<upgrade::ProviderInterface>& provider);

        ::grpc::Status Scan(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::protobuf::PackageSource* request,
            ::cc::platform::upgrade::protobuf::PackageCatalogue* response) override;

        ::grpc::Status ListSources(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::upgrade::protobuf::PackageSources* response) override;

        ::grpc::Status ListAvailable(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::protobuf::PackageSource* request,
            ::cc::platform::upgrade::protobuf::PackageCatalogue* response) override;

        ::grpc::Status BestAvailable(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::protobuf::PackageSource* request,
            ::cc::platform::upgrade::protobuf::PackageInfo* response) override;

        ::grpc::Status Install(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::protobuf::InstallRequest* request,
            ::cc::platform::upgrade::protobuf::PackageInfo* response) override;

        ::grpc::Status Finalize(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const ::cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::upgrade::protobuf::Signal>* writer) override;

    protected:
        std::shared_ptr<upgrade::ProviderInterface> provider;
    };
}  // namespace upgrade::grpc
