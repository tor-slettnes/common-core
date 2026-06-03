// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.c++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-grpc-requesthandler.h++"
#include "switchboard-grpc-signalqueue.h++"
#include "switchboard-types.h++"
#include "logging/logging.h++"
#include "protobuf-switchboard-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::switchboard::grpc
{
    //--------------------------------------------------------------------------
    // gRPC servicer methods

    RequestHandler::RequestHandler(
        const std::shared_ptr<Provider> &api_provider)
        : Super(),
          provider(api_provider)
    {
    }

    ::grpc::Status RequestHandler::GetSwitches(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchSelection *request,
        switchboard::protobuf::SwitchMap *reply)
    {
        try
        {
            cc::protobuf::encode(
                this->provider->get_selected_switches(
                    cc::protobuf::decoded<switchboard::SwitchSelection>(*request)),
                reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::ClearSwitches(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::ClearSwitchesRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            bool cleared = this->provider->clear_switches(
                request->reload());
            reply->set_value(cleared);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
                request->has_replace_specifications() ? request->replace_specifications() : false,
                request->has_replace_statuses() ? request->replace_statuses() : true,
                request->has_invoke_interceptors()
                    ? cc::protobuf::decoded<switchboard::InvocationStyle>(request->invoke_interceptors())
                    : switchboard::InvocationStyle::INDIRECT);
            reply->set_import_count(count);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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

            core::types::KeyValueMap declarations = this->provider->export_switches(
                selection,
                request->has_include_specifications() ? request->include_specifications() : false,
                request->has_include_statuses() ? request->include_statuses() : true);

            cc::protobuf::encode(declarations, reply->mutable_declarations());
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetSpecifications(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchSelection *request,
        switchboard::protobuf::SpecificationMap *reply)
    {
        try
        {
            auto &specmap = *reply->mutable_map();
            for (const auto &[name, sw] : provider->get_selected_switches(
                     cc::protobuf::decoded<switchboard::SwitchSelection>(*request)))
            {
                cc::protobuf::encode(*sw->spec(), &specmap[name]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetDependencyStatuses(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::DependencyStatusMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->dependency_statuses(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::AddInterceptor(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::AddInterceptorRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            InterceptorSessionID session_id = this->peer(context);
            InterceptorName name = request->interceptor_name();
            InterceptorName key = this->interceptor_key(name, session_id);

            std::scoped_lock lck(this->interceptor_sessions_mutex);

            Invocation invocation = [=](SwitchRef sw, State state) {
                this->on_intercept(name, session_id, sw, state);
            };

            InterceptorRef interceptor = cc::protobuf::decoded<InterceptorRef>(
                request->spec(),  // proto
                key,              // name
                session_id,       // owner
                invocation);      // invocation

            SwitchSelection selection = cc::protobuf::decoded<SwitchSelection>(
                request->switch_selection());

            bool added = this->provider->add_interceptor(
                interceptor,           // interceptor
                selection,             // switch_selection
                request->immediate(),  // immediate
                request->future());    // future

            reply->set_value(added);
            this->interceptor_sessions[session_id].registrations.insert(key);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::RemoveInterceptor(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::RemoveInterceptorRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            InterceptorName name = request->interceptor_name();
            InterceptorSessionID session_id = this->peer(context);
            InterceptorName key = this->interceptor_key(name, session_id);

            std::optional<SwitchSelection> selection;
            if (request->has_switch_selection())
            {
                selection = cc::protobuf::decoded<SwitchSelection>(
                    request->switch_selection());
            }

            // First we remove the intereceptor
            bool removed = this->provider->remove_interceptor(key, selection);

            // Next, we remove session-specific callback information
            std::scoped_lock lck(this->interceptor_sessions_mutex);
            if (InterceptorSession *session = this->interceptor_sessions.get_ptr(session_id))
            {
                if (!request->has_switch_selection())
                {
                    // Unless we removed the interceptor only from some
                    // switches, we now also remove the interceptor name from
                    // the list of interceptors to be cleaned up at the end of
                    // this session.
                    session->registrations.erase(key);
                }

                if (request->abandon_pending())
                {
                    // Remove any pending invocations of this interceptor
                    auto it = session->pending.begin();
                    while (it != session->pending.end())
                    {
                        const InterceptorName &candidate_name = it->first.first;
                        if (candidate_name == name)
                        {
                            it = session->pending.erase(it);
                        }
                        else
                        {
                            it++;
                        }
                    }
                }
            }

            reply->set_value(removed);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
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

            InterceptorName key = this->interceptor_key(
                request->interceptor_name(),
                this->peer(context));

            InterceptorRef icept = sw->get_interceptor(key, true);
            std::future<void> future = icept->invoke(
                sw,
                cc::protobuf::decoded<switchboard::State>(request->state()));

            try
            {
                future.wait();
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::Intercept(
        ::grpc::ServerContext *context,
        InterceptorStream *stream)
    {
        InterceptorSessionID session_id = create_session(context, stream);
        logf_info("Startng interceptor session: %s", session_id);

        using switchboard::protobuf::InterceptorResult;
        InterceptorResult result;

        while (stream->Read(&result))
        {
            this->on_intercept_response(result.switch_name(),
                                        result.interceptor_name(),
                                        session_id,
                                        result);
        }

        logf_info("Ending interceptor session: %s", session_id);
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

            InvocationStyle invoke_interceptors =
                request->has_invoke_interceptors()
                    ? cc::protobuf::decoded<switchboard::InvocationStyle>(
                          request->invoke_interceptors())
                    : InvocationStyle::DEFAULT;

            CascadeStyle cascade_descendants =
                request->has_cascade_descendants()
                    ? cc::protobuf::decoded<switchboard::CascadeStyle>(
                          request->cascade_descendants())
                    : CascadeStyle::ASYNC;

            bool updated = sw->set_target(
                cc::protobuf::decoded<switchboard::State>(
                    request->target_state()),
                cc::protobuf::decoded_optional_shared<core::status::Error>(
                    request->has_error(),
                    request->error()),
                cc::protobuf::decoded<core::types::KeyValueMap>(
                    request->attributes()),
                request->clear_existing(),
                invoke_interceptors,
                cascade_descendants,
                request->reenter(),
                cc::protobuf::decoded<switchboard::ExceptionHandling>(
                    request->on_cancel()),
                cc::protobuf::decoded<switchboard::ExceptionHandling>(
                    request->on_error()));

            reply->set_updated(updated);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetAttributes(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::GetAttributesRequest *request,
        switchboard::protobuf::GetAttributesResponse *reply)
    {
        try
        {
            SwitchRef sw = provider->get_switch(request->switch_name(), true);

            cc::protobuf::encode(
                sw->get_attributes(request->inherit()),
                reply->mutable_attributes());

            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetStatuses(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchSelection *request,
        switchboard::protobuf::StatusMap *reply)
    {
        try
        {
            auto &statusmap = *reply->mutable_map();
            for (const auto &[name, sw] :
                 provider->get_selected_switches(
                     cc::protobuf::decoded<switchboard::SwitchSelection>(*request)))
            {
                cc::protobuf::encode(*sw->status(), &statusmap[name]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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
            for (const auto &[switch_name, status] : sw->culprits(expected_position))
            {
                cc::protobuf::encode(*status, &statusmap[switch_name]);
            }
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
        }
    }

    ::grpc::Status RequestHandler::GetErrors(
        ::grpc::ServerContext *context,
        const switchboard::protobuf::SwitchIdentifier *request,
        switchboard::protobuf::ErrorMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            cc::protobuf::encode(sw->errors(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, this->peer(context));
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

    RequestHandler::InterceptorSessionID RequestHandler::create_session(
        ::grpc::ServerContext *context,
        InterceptorStream *stream)
    {
        InterceptorSessionID session_id = this->peer(context);
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
        const InterceptorSessionID &session_id)
    {
        std::scoped_lock lck(interceptor_sessions_mutex);
        if (auto nh = this->interceptor_sessions.extract(session_id))
        {
            for (const InterceptorName &key : nh.mapped().registrations)
            {
                this->provider->remove_interceptor(key);
            }
        }
    }

    void RequestHandler::on_intercept(
        const InterceptorName &interceptor_name,
        const InterceptorSessionID &session_id,
        SwitchRef sw,
        State state)
    {
        auto future_result = this->invoke_client_interceptor(
            interceptor_name,
            session_id,
            sw,
            state);

        // Wait for response from client
        if (future_result.valid())
        {
            switchboard::protobuf::InterceptorResult result = future_result.get();
            cc::grpc::Status status(result.error());
            status.throw_if_error();
        }
    }

    RequestHandler::FutureResult RequestHandler::invoke_client_interceptor(
        const InterceptorName &interceptor_name,
        const InterceptorSessionID &session_id,
        SwitchRef sw,
        State state)
    {
        std::scoped_lock lck(this->interceptor_sessions_mutex);
        if (auto *session = this->interceptor_sessions.get_ptr(session_id))
        {
            if (!session->context->IsCancelled())
            {
                auto [it, inserted] = session->pending.insert_or_assign(
                    {interceptor_name, sw->name()},
                    PromisedResult());

                FutureResult future = it->second.get_future();

                // Create an Invocation message for the client.
                switchboard::protobuf::InterceptorInvocation invocation;
                invocation.set_switch_name(sw->name());
                invocation.set_interceptor_name(interceptor_name);
                invocation.set_state(
                    cc::protobuf::encoded<switchboard::protobuf::State>(
                        state));
                cc::protobuf::encode(sw->get_attributes(true),
                                     invocation.mutable_cascaded_attributes());

                session->stream->Write(invocation);
                return future;
            }
        }
        return {};
    }

    void RequestHandler::on_intercept_response(
        const SwitchName &switch_name,
        const InterceptorName &interceptor_name,
        const InterceptorSessionID &session_id,
        const switchboard::protobuf::InterceptorResult &result)
    {
        // std::string key = this->pending_key(sw->name(), interceptor_name);
        if (InterceptorSession *session = this->interceptor_sessions.get_ptr(session_id))
        {
            if (auto nh = session->pending.extract({interceptor_name, switch_name}))
            {
                nh.mapped().set_value(result);
            }
        }
    }

    InterceptorName RequestHandler::interceptor_key(
        const InterceptorName &interceptor_name,
        const InterceptorSessionID &session_id) const
    {
        return interceptor_name + "@" + session_id;
    }

}  // namespace cc::platform::switchboard::grpc
