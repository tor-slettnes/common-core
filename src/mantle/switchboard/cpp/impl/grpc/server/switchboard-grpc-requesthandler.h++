// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.h++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "switchboard.h++"
#include "grpc-signalrequesthandler.h++"
#include "types/create-shared.h++"
#include "types/valuemap.h++"

#include "cc/platform/switchboard/grpc/switchboard_service.grpc.pb.h"

#include <future>

namespace switchboard
{
    using namespace cc::platform::switchboard;
}

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

        using InterceptorWriter = ::grpc::ServerWriter<
            switchboard::protobuf::InterceptorInvocation>;

        using InterceptorStream = ::grpc::ServerReaderWriter<
            switchboard::protobuf::InterceptorInvocation,
            switchboard::protobuf::InterceptorUpdate>;

        using InterceptorSessionID = std::uint64_t;
        using InterceptorInvocationID = std::string;

        struct InterceptorSession
        {
            ::grpc::ServerContext *context;
            InterceptorStream *stream;
            std::set<std::pair<SwitchRef, InterceptorName>> registrations;
        };

        using InterceptorSessionsMap = core::types::ValueMap<
            InterceptorSessionID,
            InterceptorSession>;

        // struct PendingIntercept
        // {
        //     InterceptorSessionID session_id;
        //     SwitchName switch_name;
        //     IntercptorName interceptor_name;
        //     std::promise<switchboard::protobuf::InterceptorResult> result_promise;
        // };

        using PendingInterceptsMap = std::map<
            std::pair<SwitchName, InterceptorName>,
            std::promise<switchboard::protobuf::InterceptorResult>>;


    protected:
        RequestHandler(
            const std::shared_ptr<switchboard::Provider> &api_provider);

    public:
        ::grpc::Status GetSwitches(
            ::grpc::ServerContext *context,
            const ::google::protobuf::Empty *request,
            switchboard::protobuf::SwitchMap *reply) override;

        ::grpc::Status GetSwitch(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::SwitchInfo *reply) override;

        ::grpc::Status AddSwitch(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::AddSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveSwitch(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::RemoveSwitchRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status ImportSwitches(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::ImportRequest *request,
            switchboard::protobuf::ImportResponse *reply) override;

        ::grpc::Status ExportSwitches(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::ExportRequest *request,
            switchboard::protobuf::ExportResponse *reply) override;

        ::grpc::Status SetSpecification(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SetSpecificationRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetSpecifications(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifiers *request,
            switchboard::protobuf::SpecificationMap *reply) override;

        ::grpc::Status AddDependency(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::AddDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status RemoveDependency(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::RemoveDependencyRequest *request,
            ::google::protobuf::BoolValue *reply) override;

        ::grpc::Status GetDependencies(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::DependencyMap *reply) override;

        ::grpc::Status GetAncestors(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::SwitchIdentifiers *reply) override;

        ::grpc::Status GetDescendants(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::SwitchIdentifiers *reply) override;

        ::grpc::Status GetInterceptors(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::InterceptorMap *reply) override;

        ::grpc::Status InvokeInterceptor(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::InterceptorInvocation *request,
            switchboard::protobuf::InterceptorResult *reply) override;

        ::grpc::Status Intercept(
            ::grpc::ServerContext *context,
            InterceptorStream *stream) override;

        ::grpc::Status SetTarget(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SetTargetRequest *request,
            switchboard::protobuf::SetTargetResponse *reply) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SetAttributesRequest *request,
            switchboard::protobuf::SetAttributesResponse *reply) override;

        ::grpc::Status GetStatuses(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifiers *request,
            switchboard::protobuf::StatusMap *reply) override;

        ::grpc::Status GetCulprits(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::CulpritsQuery *request,
            switchboard::protobuf::StatusMap *reply) override;

        ::grpc::Status GetErrors(
            ::grpc::ServerContext *context,
            const switchboard::protobuf::SwitchIdentifier *request,
            switchboard::protobuf::ErrorMap *reply) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext *context,
            const cc::protobuf::signal::Filter *request,
            ::grpc::ServerWriter<switchboard::protobuf::Signal> *writer) override;

    private:
        SwitchMap get_switches(
            const ::google::protobuf::RepeatedPtrField<std::string> &switch_names) const;

        InterceptorSessionID create_session(
            ::grpc::ServerContext *context,
            InterceptorStream *stream);

        void end_session(
            InterceptorSessionID session_id);

        void add_intercept_registration(
            InterceptorSessionID session_id,
            SwitchRef sw,
            const InterceptorName &interceptor_name,
            const switchboard::protobuf::InterceptorRegistration &reg);

        void remove_intercept_registration(
            InterceptorSessionID session_id,
            SwitchRef sw,
            const InterceptorName &interceptor_name,
            const switchboard::protobuf::InterceptorDeregistration &dereg);

        void on_intercept(
            InterceptorSessionID session_id,
            const InterceptorName &interceptor_name,
            SwitchRef sw,
            State state);

        std::future<switchboard::protobuf::InterceptorResult> invoke_client_interceptor(
            InterceptorSessionID session_id,
            const InterceptorName &interceptor_name,
            SwitchRef sw,
            State state);

        void on_intercept_response(
            SwitchRef sw,
            const InterceptorName &interceptor_name,
            const switchboard::protobuf::InterceptorResult &result);

        std::string pending_key(
            const std::string &switch_name,
            const std::string &interceptor_name) const;

    private:
        std::shared_ptr<switchboard::Provider> provider;
        InterceptorSessionID latest_interceptor_session;
        std::mutex interceptor_sessions_mutex;
        InterceptorSessionsMap interceptor_sessions;
        PendingInterceptsMap pending_intercepts;
    };
}  // namespace switchboard::grpc
