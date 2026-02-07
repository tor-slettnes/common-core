// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.c++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-grpc-requesthandler.h++"
#include "switchboard-grpc-signalqueue.h++"
#include "logging/logging.h++"
#include "protobuf-switchboard-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace switchboard::grpc
{
    //--------------------------------------------------------------------------
    // gRPC servicer methods

    RequestHandler::RequestHandler(
        const std::shared_ptr<Provider> &api_provider)
        : Super(),
          provider(api_provider),
          latest_interceptor_session(0)
    {
    }

    ::grpc::Status RequestHandler::GetSwitches(
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        switchboard::protobuf::SwitchMap *reply)
    {
        try
        {
            cc::protobuf::encode(this->provider->get_switches(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetSwitch(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::SwitchInfo *reply)
    {
        try
        {
            if (SwitchRef sw = this->provider->get_switch(request->switch_name()))
            {
                cc::protobuf::encode(*sw, reply);
            }
            else
            {
                return {};
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::AddSwitch(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::AddSwitchRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            auto [sw, inserted] = this->provider->add_switch(
                request->switch_name(),
                request->active());
            reply->set_value(inserted);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::RemoveSwitch(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::RemoveSwitchRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            bool removed = this->provider->remove_switch(
                request->switch_name(),
                request->propagate());
            reply->set_value(removed);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ImportSwitches(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::ImportRequest *request,
        switchboard::protobuf::ImportResponse *reply)
    {
        try
        {
            uint count = this->provider->import_switches(
                cc::protobuf::decoded<core::types::KeyValueMap>(request->declarations()),
                request->replace_specifications(),
                request->replace_statuses());
            reply->set_import_count(count);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::ExportSwitches(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::ExportRequest *request,
        switchboard::protobuf::ExportResponse *reply)
    {
        try
        {
            std::optional<SwitchSelection> selection;
            if (request->has_selection())
            {
                selection = cc::protobuf::decoded<SwitchSelection>(
                    request->selection());
            }

            cc::protobuf::encode(
                this->provider->export_switches(
                    selection,
                    request->include_specifications(),
                    request->include_statuses()),
                reply->mutable_declarations());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::SetSpecification(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SetSpecificationRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            if (auto sw = this->provider->get_switch(request->switch_name()))
            {
                const switchboard::protobuf::Specification &spec = request->spec();
                sw->update_spec(
                    spec.has_is_primary() ? spec.is_primary() : std::optional<bool>(),
                    cc::protobuf::decoded<SwitchAliases>(spec.aliases()),
                    request->replace_aliases(),
                    cc::protobuf::decoded<LocalizationMap>(spec.localizations()),
                    request->replace_localizations(),
                    cc::protobuf::decoded<DependencyMap>(spec.dependencies(), this->provider),
                    request->replace_dependencies(),
                    cc::protobuf::decoded<InterceptorMap>(spec.interceptors()),
                    request->replace_interceptors(),
                    request->update_state());

                reply->set_value(true);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetSpecifications(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifiers *request,
        switchboard::protobuf::SpecificationMap *reply)
    {
        try
        {
            auto &specmap = *reply->mutable_map();
            for (const auto &[name, sw] : this->get_switches(request->switch_names()))
            {
                cc::protobuf::encode(*sw->spec(), &specmap[name]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::AddDependency(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::AddDependencyRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            DependencyRef dep = cc::protobuf::decoded<DependencyRef>(
                request->dependency(),
                this->provider,
                request->predecessor_name());

            bool added = sw->add_dependency(
                dep,
                request->has_allow_update() ? request->allow_update() : true,
                request->has_reevaluate() ? request->reevaluate() : true);

            reply->set_value(added);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::RemoveDependency(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::RemoveDependencyRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            if (SwitchRef sw = this->provider->get_switch(request->switch_name()))
            {
                bool reevaluate = request->has_reevaluate() ? request->reevaluate() : true;
                bool removed = sw->remove_dependency(request->predecessor_name(), reevaluate);
                reply->set_value(removed);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetDependencies(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::DependencyMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->dependencies(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetAncestors(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::SwitchIdentifiers *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->get_ancestors(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetDescendants(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::SwitchIdentifiers *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->get_descendants(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetInterceptors(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::InterceptorMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->interceptors(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::InvokeInterceptor(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::InterceptorInvocation *request,
        switchboard::protobuf::InterceptorResult *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            InterceptorRef icept = sw->get_interceptor(request->interceptor_name(), true);

            try
            {
                icept->invoke(
                    sw,
                    cc::protobuf::decoded<switchboard::State>(request->state()));
            }
            catch (...)
            {
                cc::protobuf::encode_shared(
                    core::exception::map_to_error(std::current_exception()),
                    reply->mutable_error());
            }

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::Intercept(
        ::grpc::ServerContext *context,
        InterceptorStream *stream)
    {
        InterceptorSessionID session_id = this->create_session(context, stream);

        logf_debug("Startng interceptor session %d on behalf of client %s",
                   session_id,
                   context->peer());

        using switchboard::protobuf::InterceptorUpdate;
        InterceptorUpdate update;
        while (stream->Read(&update))
        {
            SwitchRef sw = this->provider->get_switch(update.switch_name(), true);
            InterceptorName name = update.interceptor_name();

            switch (update.update_type_case())
            {
            case InterceptorUpdate::kRegistration:
                this->add_intercept_registration(
                    session_id,
                    sw,
                    name,
                    update.registration());
                break;

            case InterceptorUpdate::kDeregistration:
                this->remove_intercept_registration(
                    session_id,
                    sw,
                    name,
                    update.deregistration());
                break;

            case InterceptorUpdate::kInvocationResult:
                this->on_intercept_response(
                    sw,
                    name,
                    update.invocation_result());
                break;
            }
        }
        logf_debug("Ending interceptor session %d on behalf of client %s",
                   session_id,
                   context->peer());

        this->end_session(session_id);
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::SetTarget(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SetTargetRequest *request,
        switchboard::protobuf::SetTargetResponse *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            bool updated = sw->set_target(
                cc::protobuf::decoded<switchboard::State>(request->target_state()),
                cc::protobuf::decoded_shared<core::status::Error>(request->error()),
                cc::protobuf::decoded<core::types::KeyValueMap>(request->attributes()),
                request->clear_existing(),
                request->with_interceptors(),
                request->trigger_descendants(),
                request->reevaluate(),
                cc::protobuf::decoded<switchboard::ExceptionHandling>(request->on_cancel()),
                cc::protobuf::decoded<switchboard::ExceptionHandling>(request->on_error()));

            reply->set_updated(updated);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::SetAttributes(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SetAttributesRequest *request,
        switchboard::protobuf::SetAttributesResponse *reply)
    {
        try
        {
            SwitchRef sw = provider->get_switch(request->switch_name(), true);
            bool updated = sw->set_attributes(
                cc::protobuf::decoded<core::types::KeyValueMap>(request->attributes()),
                request->clear_existing());
            reply->set_updated(updated);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetStatuses(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifiers *request,
        switchboard::protobuf::StatusMap *reply)
    {
        try
        {
            auto &statusmap = *reply->mutable_map();
            for (const auto &[name, sw] : this->get_switches(request->switch_names()))
            {
                cc::protobuf::encode(*sw->status(), &statusmap[name]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetCulprits(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::CulpritsQuery *request,
        switchboard::protobuf::StatusMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            bool expected_position = request->has_expected()
                                       ? request->expected()
                                       : true;

            auto &statusmap = *reply->mutable_map();
            for (const auto &[sw, state] : sw->culprits(expected_position))
            {
                cc::protobuf::encode(*sw->status(), &statusmap[sw->name()]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetErrors(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::ErrorMap *reply)
    {
        try
        {
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::Watch(
        ::grpc::ServerContext *context,
        const cc::protobuf::signal::Filter *request,
        ::grpc::ServerWriter<switchboard::protobuf::Signal> *writer)
    {
        return this->stream_signals<switchboard::protobuf::Signal, SignalQueue>(
            context,
            request,
            writer);
    }

    SwitchMap RequestHandler::get_switches(
        const google::protobuf::RepeatedPtrField<std::string> &switch_names) const
    {
        SwitchMap switches;
        if (switch_names.empty())
        {
            switches = this->provider->get_switches();
        }
        else
        {
            for (const std::string &name : switch_names)
            {
                if (const auto &sw = this->provider->get_switch(name))
                {
                    switches.insert_or_assign(name, sw);
                }
            }
        }

        return switches;
    }

    RequestHandler::InterceptorSessionID RequestHandler::create_session(
        ::grpc::ServerContext *context,
        InterceptorStream *stream)
    {
        InterceptorSessionID session_id = ++this->latest_interceptor_session;
        logf_info("Starting interceptor session %d on behalf of client %s",
                  session_id,
                  context->peer());

        std::scoped_lock lck(this->interceptor_sessions_mutex);
        this->interceptor_sessions.insert_or_assign(
            session_id,
            InterceptorSession({
                .context = context,
                .stream = stream,
            }));

        return session_id;
    }

    void RequestHandler::end_session(
        InterceptorSessionID session_id)
    {
        std::scoped_lock lck(interceptor_sessions_mutex);
        if (auto nh = this->interceptor_sessions.extract(session_id))
        {
            for (const auto &[sw, interceptor_name] : nh.mapped().registrations)
            {
                sw->remove_interceptor(interceptor_name);
                // std::string key = this->pending_key(sw->name(), interceptor_name);
                this->pending_intercepts.erase({sw->name(), interceptor_name});
            }
        }
    }

    void RequestHandler::add_intercept_registration(
        InterceptorSessionID session_id,
        SwitchRef sw,
        const InterceptorName &interceptor_name,
        const switchboard::protobuf::InterceptorRegistration &reg)
    {
        std::scoped_lock lck(this->interceptor_sessions_mutex);
        if (InterceptorSession *session = this->interceptor_sessions.get_ptr(session_id))
        {
            session->registrations.insert({sw, interceptor_name});
        }

        const switchboard::protobuf::InterceptorSpec &spec = reg.spec();

        using namespace std::placeholders;
        auto icept = Interceptor::create_shared(
            interceptor_name,                                            // name
            "gRPC client session #" + std::to_string(session_id),        // owner
            std::bind(&This::on_intercept,                               // |
                      this,                                              // |
                      session_id,                                        // | invocation
                      interceptor_name,                                  // |
                      _1,                                                // |
                      _2),                                               // |
            cc::protobuf::decoded<StateSet>(spec.state_transitions()),   // state_transitions
            cc::protobuf::decoded<InterceptorPhase>(spec.phase()),       // phase
            spec.asynchronous(),                                         // asynchronous
            spec.rerun(),                                                // rerun
            cc::protobuf::decoded<ExceptionHandling>(spec.on_cancel()),  // on_cancel
            cc::protobuf::decoded<ExceptionHandling>(spec.on_error()));  // on_error

        sw->add_interceptor(icept, reg.immediate());
    }

    void RequestHandler::remove_intercept_registration(
        InterceptorSessionID session_id,
        SwitchRef sw,
        const InterceptorName &interceptor_name,
        const switchboard::protobuf::InterceptorDeregistration &dereg)
    {
        sw->remove_interceptor(interceptor_name);

        std::scoped_lock lck(this->interceptor_sessions_mutex);
        if (InterceptorSession *session = this->interceptor_sessions.get_ptr(session_id))
        {
            session->registrations.erase({sw, interceptor_name});
        }
    }

    void RequestHandler::on_intercept(
        InterceptorSessionID session_id,
        const InterceptorName &interceptor_name,
        SwitchRef sw,
        State state)
    {
        auto future_result = this->invoke_client_interceptor(
            session_id,
            interceptor_name,
            sw,
            state);

        // Wait for response from client
        if (future_result.valid())
        {
            switchboard::protobuf::InterceptorResult result = future_result.get();
            core::grpc::Status status(result.error());
            status.throw_if_error();
        }
    }

    std::future<switchboard::protobuf::InterceptorResult> RequestHandler::invoke_client_interceptor(
        InterceptorSessionID session_id,
        const InterceptorName &interceptor_name,
        SwitchRef sw,
        State state)
    {
        std::scoped_lock lck(this->interceptor_sessions_mutex);
        if (auto *session = this->interceptor_sessions.get_ptr(session_id))
        {
            if (!session->context->IsCancelled())
            {
                // std::string key = this->pending_key(sw->name(), interceptor_name);
                auto &promise = this->pending_intercepts[{sw->name(), interceptor_name}];
                auto future = promise.get_future();

                // Create an Invocation message for the client.
                switchboard::protobuf::InterceptorInvocation invocation;
                invocation.set_switch_name(sw->name());
                invocation.set_interceptor_name(interceptor_name);
                invocation.set_state(
                    cc::protobuf::encoded<switchboard::protobuf::State>(
                        state));

                session->stream->Write(invocation);
                return future;
            }
        }
        return {};
    }

    void RequestHandler::on_intercept_response(
        SwitchRef sw,
        const InterceptorName &interceptor_name,
        const switchboard::protobuf::InterceptorResult &result)
    {
        // std::string key = this->pending_key(sw->name(), interceptor_name);
        if (auto nh = this->pending_intercepts.extract({sw->name(), interceptor_name}))
        {
            nh.mapped().set_value(result);
        }
    }

    std::string RequestHandler::pending_key(
        const std::string &switch_name,
        const std::string &interceptor_name) const
    {
        return switch_name + "/" + interceptor_name;
    }

}  // namespace switchboard::grpc
