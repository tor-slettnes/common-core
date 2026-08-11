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
#include <tuple>

namespace cc::platform::switchboard
{
    using namespace cc::platform::switchboard;
}

namespace cc::platform::switchboard::grpc
{
    //==========================================================================
    /// @class RequestHandler
    /// @brief Process requests from Switchboard clients.

    class RequestHandler
        : public cc::grpc::SignalRequestHandler<cc::platform::switchboard::grpc::Switchboard>,
          public core::types::enable_create_shared<RequestHandler>
    {
        using This = RequestHandler;
        using Super = cc::grpc::SignalRequestHandler<cc::platform::switchboard::grpc::Switchboard>;

        using InterceptorWriter = ::grpc::ServerWriter<
            switchboard::protobuf::InterceptorInvocation>;

        using InterceptorStream = ::grpc::ServerReaderWriter<
            switchboard::protobuf::InterceptorInvocation,
            switchboard::protobuf::InterceptorResult>;

        using InterceptorSessionID = std::string;
        using PromisedResult = std::promise<switchboard::protobuf::InterceptorResult>;
        using FutureResult = std::future<switchboard::protobuf::InterceptorResult>;

        using PendingInterceptsMap = std::map<
            std::pair<InterceptorName, SwitchName>,
            PromisedResult>;

        struct InterceptorSession
        {
            ::grpc::ServerContext* context;
            InterceptorStream* stream;
            std::set<InterceptorName> registrations;
            PendingInterceptsMap pending;
        };

        using InterceptorSessionsMap = core::types::ValueMap<
            InterceptorSessionID,
            InterceptorSession>;

    protected:
        RequestHandler(
            const std::shared_ptr<switchboard::Provider>& api_provider);

    public:
        ::grpc::Status ServiceCheck(
            ::grpc::ServerContext* context,
            const ::google::protobuf::Empty* request,
            ServiceCheckResponse *response) override;

        ::grpc::Status GetSwitches(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchSelection* request,
            switchboard::protobuf::SwitchMap* reply) override;

        ::grpc::Status GetSwitch(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::SwitchInfo* reply) override;

        ::grpc::Status AddSwitch(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::AddSwitchRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status RemoveSwitch(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::RemoveSwitchRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status ClearSwitches(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::ClearSwitchesRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status ImportSwitches(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::ImportRequest* request,
            switchboard::protobuf::ImportResponse* reply) override;

        ::grpc::Status ExportSwitches(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::ExportRequest* request,
            switchboard::protobuf::ExportResponse* reply) override;

        ::grpc::Status SetSpecification(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SetSpecificationRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status GetSpecifications(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchSelection* request,
            switchboard::protobuf::SpecificationMap* reply) override;

        ::grpc::Status AddDependency(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::AddDependencyRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status RemoveDependency(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::RemoveDependencyRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status GetDependencies(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::DependencyMap* reply) override;

        ::grpc::Status GetAncestors(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::SwitchIdentifiers* reply) override;

        ::grpc::Status GetDescendants(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::SwitchIdentifiers* reply) override;

        ::grpc::Status GetDependencyStatuses(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::DependencyStatusMap* reply) override;

        ::grpc::Status AddInterceptor(
            ::grpc::ServerContext* context,
            const cc::platform::switchboard::protobuf::AddInterceptorRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status RemoveInterceptor(
            ::grpc::ServerContext* context,
            const cc::platform::switchboard::protobuf::RemoveInterceptorRequest* request,
            ::google::protobuf::BoolValue* reply) override;

        ::grpc::Status GetInterceptors(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::InterceptorMap* reply) override;

        ::grpc::Status InvokeInterceptor(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::InterceptorInvocation* request,
            switchboard::protobuf::InterceptorResult* reply) override;

        ::grpc::Status Intercept(
            ::grpc::ServerContext* context,
            InterceptorStream* stream) override;

        ::grpc::Status SetTarget(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SetTargetRequest* request,
            switchboard::protobuf::SetTargetResponse* reply) override;

        ::grpc::Status GetAttributes(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::GetAttributesRequest* request,
            switchboard::protobuf::GetAttributesResponse* reply) override;

        ::grpc::Status SetAttributes(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SetAttributesRequest* request,
            switchboard::protobuf::SetAttributesResponse* reply) override;

        ::grpc::Status GetStatuses(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchSelection* request,
            switchboard::protobuf::StatusMap* reply) override;

        ::grpc::Status GetCulprits(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::CulpritsQuery* request,
            switchboard::protobuf::StatusMap* reply) override;

        ::grpc::Status GetErrors(
            ::grpc::ServerContext* context,
            const switchboard::protobuf::SwitchIdentifier* request,
            switchboard::protobuf::ErrorMap* reply) override;

        ::grpc::Status Watch(
            ::grpc::ServerContext* context,
            const cc::protobuf::signal::Filter* request,
            ::grpc::ServerWriter<switchboard::protobuf::Signal>* writer) override;

    private:
        InterceptorSessionID create_session(
            ::grpc::ServerContext* context,
            InterceptorStream* stream);

        void end_session(
            const InterceptorSessionID& session_id);

        void on_intercept(
            const InterceptorName& interceptor_name,
            const std::string& peer,
            SwitchRef sw,
            State state);

        FutureResult invoke_client_interceptor(
            const InterceptorName& interceptor_name,
            const InterceptorSessionID& session_id,
            SwitchRef sw,
            State state);

        void on_intercept_response(
            const SwitchName& switch_name,
            const InterceptorName& interceptor_name,
            const InterceptorSessionID& session_id,
            const switchboard::protobuf::InterceptorResult& result);

        InterceptorName interceptor_key(
            const InterceptorName& interceptor_name,
            const InterceptorSessionID& session_id) const;

    private:
        std::shared_ptr<switchboard::Provider> provider;
        std::mutex interceptor_sessions_mutex;
        InterceptorSessionsMap interceptor_sessions;
    };
}  // namespace cc::platform::switchboard::grpc
