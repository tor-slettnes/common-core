// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.h++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "grpc-signalrequesthandler.h++"
#include "cc/switchboard/switchboard.grpc.pb.h"  // generated from `switchboard.proto`
#include "types/create-shared.h++"

namespace switchboard::grpc
{
    //==========================================================================
    /// @class RequestHandler
    /// @brief Process requests from Switchboard clients.

    class RequestHandler
        : public core::grpc::SignalRequestHandler<cc::switchboard::Switchboard>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = core::grpc::SignalRequestHandler<cc::switchboard::Switchboard>;

        using InterceptorStream = ::grpc::ServerReaderWriter<
            cc::switchboard::InterceptorInvocation,
            cc::switchboard::InterceptorResult>;

    protected:
        RequestHandler(
            const std::shared_ptr<switchboard::Provider> &api_provider);

    public:
        ::grpc::Status GetSwitches(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            cc::switchboard::SwitchMap *reply) override;

        ::grpc::Status GetSwitch(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchInfo *reply) override;

        ::grpc::Status AddSwitch(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveSwitch(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status SetSpecification(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetSpecificationRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetSpecifications(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifiers *request,
            cc::switchboard::SpecificationMap *reply) override;

        ::grpc::Status AddDependency(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveDependency(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetDependencies(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::DependencyMap *reply) override;

        ::grpc::Status GetAncestors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchIdentifiers *reply) override;

        ::grpc::Status GetDescendents(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchIdentifiers *reply) override;

        ::grpc::Status AddInterceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveInterceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetInterceptors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::InterceptorMap *reply) override;

        ::grpc::Status InvokeInterceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::InterceptorInvocation *request,
            cc::switchboard::InterceptorResult *reply) override;

        ::grpc::Status Intercept(
            ::grpc::ServerContext *context,
            InterceptorStream *stream) override;

        ::grpc::Status SetTarget(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetTargetRequest *request,
            cc::switchboard::SetTargetResponse *reply) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetAttributesRequest *request,
            cc::switchboard::SetAttributesResponse *reply) override;

        ::grpc::Status GetStatuses(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifiers *request,
            cc::switchboard::StatusMap *reply) override;

        ::grpc::Status GetCulprits(
            ::grpc::ServerContext *context,
            const cc::switchboard::CulpritsQuery *request,
            cc::switchboard::StatusMap *reply) override;

        ::grpc::Status GetErrors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::ErrorMap *reply) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext *context,
            const cc::protobuf::signal::Filter *request,
            ::grpc::ServerWriter<cc::switchboard::Signal> *writer) override;

    private:

        SwitchMap get_switches(
            const ::google::protobuf::RepeatedPtrField<std::string> &switch_names);

    private:
        std::shared_ptr<switchboard::Provider> provider;

    };
}  // namespace switchboard::grpc
