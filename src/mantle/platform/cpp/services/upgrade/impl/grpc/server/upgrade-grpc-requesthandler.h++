// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-requesthandler.h++
/// @brief Handle Upgrade gRPC service requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade.grpc.pb.h"  // Generated from `upgrade.proto`
#include "upgrade-base.h++"

#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"

namespace platform::upgrade::grpc
{
    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::platform::upgrade::Upgrade>,
          public core::types::enable_create_shared<RequestHandler>
    {
        // Convencience aliases
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::platform::upgrade::Upgrade>;

    protected:
        RequestHandler();
        RequestHandler(const std::shared_ptr<upgrade::ProviderInterface>& provider);

        ::grpc::Status scan(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::PackageSource* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status list_available(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::grpc::ServerWriter<::cc::platform::upgrade::PackageInfo>* writer) override;

        ::grpc::Status best_available(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::cc::platform::upgrade::PackageInfo* response) override;

        ::grpc::Status install(
            ::grpc::ServerContext* context,
            const ::cc::platform::upgrade::PackageSource* request,
            ::cc::platform::upgrade::PackageInfo* response) override;

        ::grpc::Status finalize(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ::google::protobuf::Empty* response) override;

        ::grpc::Status watch(
            ::grpc::ServerContext* context,
            const ::cc::signal::Filter* filter,
            ::grpc::ServerWriter<::cc::platform::upgrade::Signal>* writer) override;

    protected:
        std::shared_ptr<upgrade::ProviderInterface> provider;
    };
}  // namespace platform::upgrade::grpc
