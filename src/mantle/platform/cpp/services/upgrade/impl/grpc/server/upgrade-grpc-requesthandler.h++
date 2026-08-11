// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-requesthandler.h++
/// @brief Handle Upgrade gRPC service requests
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/upgrade/grpc/upgrade_service.grpc.pb.h"
#include "upgrade-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace cc::platform::upgrade::grpc
{
    /// Process requests from Upgrade gRPC clients
    class RequestHandler
        : public cc::grpc::SignalRequestHandler<platform::upgrade::grpc::Upgrade>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = cc::grpc::SignalRequestHandler<platform::upgrade::grpc::Upgrade>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<upgrade::ProviderInterface>& provider);

        ::grpc::Status ServiceCheck(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ServiceCheckResponse *response) override;

        ::grpc::Status Scan(
            ::grpc::ServerContext* context,
            const platform::upgrade::protobuf::PackageSource* request,
            platform::upgrade::protobuf::PackageCatalogue* response) override;

        ::grpc::Status ListSources(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            platform::upgrade::protobuf::PackageSources* response) override;

        ::grpc::Status ListAvailable(
            ::grpc::ServerContext* context,
            const platform::upgrade::protobuf::PackageSource* request,
            platform::upgrade::protobuf::PackageCatalogue* response) override;

        ::grpc::Status BestAvailable(
            ::grpc::ServerContext* context,
            const platform::upgrade::protobuf::PackageSource* request,
            platform::upgrade::protobuf::PackageInfo* response) override;

        ::grpc::Status Install(
            ::grpc::ServerContext* context,
            const platform::upgrade::protobuf::InstallRequest* request,
            platform::upgrade::protobuf::PackageInfo* response) override;

        ::grpc::Status Finalize(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const cc::protobuf::signal::Filter* filter,
            ::grpc::ServerWriter<platform::upgrade::protobuf::Signal>* writer) override;

    protected:
        std::shared_ptr<upgrade::ProviderInterface> provider;
    };
}  // namespace cc::platform::upgrade::grpc
