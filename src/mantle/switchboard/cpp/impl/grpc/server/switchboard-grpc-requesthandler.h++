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
        ::grpc::Status get_switches(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            cc::switchboard::SwitchMap *reply) override;

        ::grpc::Status get_switch(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchInfo *reply) override;

        ::grpc::Status add_switch(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status remove_switch(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status set_specification(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetSpecificationRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status get_specifications(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifiers *request,
            cc::switchboard::SpecificationMap *reply) override;

        ::grpc::Status add_dependency(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status remove_dependency(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status get_dependencies(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::DependencyMap *reply) override;

        ::grpc::Status get_ancestors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchIdentifiers *reply) override;

        ::grpc::Status get_descendents(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::SwitchIdentifiers *reply) override;

        ::grpc::Status add_interceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::AddInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status remove_interceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::RemoveInterceptorRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status get_interceptors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::InterceptorMap *reply) override;

        ::grpc::Status invoke_interceptor(
            ::grpc::ServerContext *context,
            const cc::switchboard::InterceptorInvocation *request,
            cc::switchboard::InterceptorResult *reply) override;

        ::grpc::Status intercept(
            ::grpc::ServerContext *context,
            InterceptorStream *stream) override;

        ::grpc::Status set_target(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetTargetRequest *request,
            cc::switchboard::SetTargetResponse *reply) override;

        ::grpc::Status set_attributes(
            ::grpc::ServerContext *context,
            const cc::switchboard::SetAttributesRequest *request,
            cc::switchboard::SetAttributesResponse *reply) override;

        ::grpc::Status get_statuses(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifiers *request,
            cc::switchboard::StatusMap *reply) override;

        ::grpc::Status get_culprits(
            ::grpc::ServerContext *context,
            const cc::switchboard::CulpritsQuery *request,
            cc::switchboard::StatusMap *reply) override;

        ::grpc::Status get_errors(
            ::grpc::ServerContext *context,
            const cc::switchboard::SwitchIdentifier *request,
            cc::switchboard::ErrorMap *reply) override;

        ::grpc::Status watch(
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
