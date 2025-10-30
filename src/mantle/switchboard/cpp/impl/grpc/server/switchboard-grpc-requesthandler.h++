// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.h++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "grpc-signalrequesthandler.h++"
#include "cc/platform/switchboard/grpc/switchboard_service.grpc.pb.h"
#include "types/create-shared.h++"

namespace switchboard::grpc
{
    //==========================================================================
    /// @class RequestHandler
    /// @brief Process requests from Switchboard clients.

    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::platform::switchboard::grpc::Switchboard>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::platform::switchboard::grpc::Switchboard>;

        using InterceptorStream = ::grpc::ServerReaderWriter<
            cc::platform::switchboard::protobuf::InterceptorInvocation,
            cc::platform::switchboard::protobuf::InterceptorResult>;

    protected:
        RequestHandler(
            const std::shared_ptr<switchboard::Provider> &api_provider);

    public:
        ::grpc::Status GetSwitches(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            cc::platform::switchboard::protobuf::SwitchMap *reply) override;

        ::grpc::Status GetSwitch(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::SwitchInfo *reply) override;

        ::grpc::Status AddSwitch(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::AddSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveSwitch(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::RemoveSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status SetSpecification(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SetSpecificationRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetSpecifications(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifiers *request,
            cc::platform::switchboard::protobuf::SpecificationMap *reply) override;

        ::grpc::Status AddDependency(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::AddDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveDependency(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::RemoveDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetDependencies(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::DependencyMap *reply) override;

        ::grpc::Status GetAncestors(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::SwitchIdentifiers *reply) override;

        ::grpc::Status GetDescendents(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::SwitchIdentifiers *reply) override;

        ::grpc::Status AddInterceptor(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::AddInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveInterceptor(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::RemoveInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetInterceptors(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::InterceptorMap *reply) override;

        ::grpc::Status InvokeInterceptor(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::InterceptorInvocation *request,
            cc::platform::switchboard::protobuf::InterceptorResult *reply) override;

        ::grpc::Status Intercept(
            ::grpc::ServerContext *context,
            InterceptorStream *stream) override;

        ::grpc::Status SetTarget(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SetTargetRequest *request,
            cc::platform::switchboard::protobuf::SetTargetResponse *reply) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SetAttributesRequest *request,
            cc::platform::switchboard::protobuf::SetAttributesResponse *reply) override;

        ::grpc::Status GetStatuses(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifiers *request,
            cc::platform::switchboard::protobuf::StatusMap *reply) override;

        ::grpc::Status GetCulprits(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::CulpritsQuery *request,
            cc::platform::switchboard::protobuf::StatusMap *reply) override;

        ::grpc::Status GetErrors(
            ::grpc::ServerContext *context,
            const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
            cc::platform::switchboard::protobuf::ErrorMap *reply) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext *context,
            const cc::protobuf::signal::Filter *request,
            ::grpc::ServerWriter<cc::platform::switchboard::protobuf::Signal> *writer) override;

    private:

        SwitchMap get_switches(
            const ::google::protobuf::RepeatedPtrField<std::string> &switch_names);

    private:
        std::shared_ptr<switchboard::Provider> provider;

    };
}  // namespace switchboard::grpc
