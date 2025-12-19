// -*- c++ -*-
//==============================================================================
/// @file switchboard-grpc-requesthandler.c++
/// @brief Base class for Switchboard servers (standalone or relay)
/// @author Tor Slettnes <tor@slett.net>
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
          provider(api_provider)
    {
    }

    ::grpc::Status RequestHandler::GetSwitches(
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        cc::platform::switchboard::protobuf::SwitchMap *reply)
    {
        try
        {
            protobuf::encode(this->provider->get_switches(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetSwitch(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::SwitchInfo *reply)
    {
        try
        {
            if (SwitchRef sw = this->provider->get_switch(request->switch_name()))
            {
                protobuf::encode(*sw, reply);
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
        const cc::platform::switchboard::protobuf::AddSwitchRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            auto [sw, inserted] = this->provider->add_switch(request->switch_name());
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
        const cc::platform::switchboard::protobuf::RemoveSwitchRequest *request,
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

    ::grpc::Status RequestHandler::SetSpecification(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SetSpecificationRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            if (auto sw = this->provider->get_switch(request->switch_name()))
            {
                const cc::platform::switchboard::protobuf::Specification &spec = request->spec();
                sw->update_spec(
                    spec.has_is_primary() ? spec.is_primary() : std::optional<bool>(),
                    protobuf::decoded<SwitchAliases>(spec.aliases()),
                    request->replace_aliases(),
                    protobuf::decoded<LocalizationMap>(spec.localizations()),
                    request->replace_localizations(),
                    protobuf::decoded<DependencyMap>(spec.dependencies(), this->provider),
                    request->replace_dependencies(),
                    protobuf::decoded<InterceptorMap>(spec.interceptors()),
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
        const cc::platform::switchboard::protobuf::SwitchIdentifiers *request,
        cc::platform::switchboard::protobuf::SpecificationMap *reply)
    {
        try
        {
            auto &specmap = *reply->mutable_map();
            for (const auto &[name, sw] : this->get_switches(request->switch_names()))
            {
                protobuf::encode(*sw->spec(), &specmap[name]);
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
        const cc::platform::switchboard::protobuf::AddDependencyRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            DependencyRef dep = protobuf::decoded<DependencyRef>(
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
        const cc::platform::switchboard::protobuf::RemoveDependencyRequest *request,
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
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::DependencyMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            protobuf::encode(sw->dependencies(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetAncestors(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::SwitchIdentifiers *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            protobuf::encode(sw->get_ancestors(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetDescendants(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::SwitchIdentifiers *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            protobuf::encode(sw->get_descendants(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::AddInterceptor(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::AddInterceptorRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            InterceptorRef icept = protobuf::decoded<InterceptorRef>(
                request->spec(),
                request->interceptor_name(),
                [](SwitchRef sw, State state) {
                    logf_info("Callback for switch=%r, state=%s", *sw, state);
                });

            bool added = sw->add_interceptor(icept, request->immediate());
            reply->set_value(added);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::RemoveInterceptor(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::RemoveInterceptorRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            bool removed = sw->remove_interceptor(request->interceptor_name());
            reply->set_value(removed);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::GetInterceptors(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::InterceptorMap *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            protobuf::encode(sw->interceptors(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::InvokeInterceptor(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::InterceptorInvocation *request,
        cc::platform::switchboard::protobuf::InterceptorResult *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            reply->set_switch_name(sw->name());

            InterceptorRef icept = sw->get_interceptor(request->interceptor_name(), true);
            reply->set_interceptor_name(icept->name());

            std::future<void> future = icept->invoke(
                sw,
                protobuf::decoded<switchboard::State>(request->state()));

            try
            {
                future.wait();
            }
            catch (...)
            {
                protobuf::encode_shared(
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
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::SetTarget(
        ::grpc::ServerContext *context,
        const cc::platform::switchboard::protobuf::SetTargetRequest *request,
        cc::platform::switchboard::protobuf::SetTargetResponse *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            bool updated = sw->set_target(
                protobuf::decoded<switchboard::State>(request->target_state()),
                protobuf::decoded_shared<core::status::Error>(request->error()),
                protobuf::decoded<core::types::KeyValueMap>(request->attributes()),
                request->clear_existing(),
                request->with_interceptors(),
                request->trigger_descendants(),
                request->reevaluate(),
                protobuf::decoded<switchboard::ExceptionHandling>(request->on_cancel()),
                protobuf::decoded<switchboard::ExceptionHandling>(request->on_error()));

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
        const cc::platform::switchboard::protobuf::SetAttributesRequest *request,
        cc::platform::switchboard::protobuf::SetAttributesResponse *reply)
    {
        try
        {
            SwitchRef sw = provider->get_switch(request->switch_name(), true);
            bool updated = sw->set_attributes(
                protobuf::decoded<core::types::KeyValueMap>(request->attributes()),
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
        const cc::platform::switchboard::protobuf::SwitchIdentifiers *request,
        cc::platform::switchboard::protobuf::StatusMap *reply)
    {
        try
        {
            auto &statusmap = *reply->mutable_map();
            for (const auto &[name, sw] : this->get_switches(request->switch_names()))
            {
                protobuf::encode(*sw->status(), &statusmap[name]);
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
        const cc::platform::switchboard::protobuf::CulpritsQuery *request,
        cc::platform::switchboard::protobuf::StatusMap *reply)
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
                protobuf::encode(*sw->status(), &statusmap[sw->name()]);
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
        const cc::platform::switchboard::protobuf::SwitchIdentifier *request,
        cc::platform::switchboard::protobuf::ErrorMap *reply)
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
        ::grpc::ServerWriter<cc::platform::switchboard::protobuf::Signal> *writer)
    {
        return this->stream_signals<cc::platform::switchboard::protobuf::Signal, SignalQueue>(
            context,
            request,
            writer);
    }

    SwitchMap RequestHandler::get_switches(
        const google::protobuf::RepeatedPtrField<std::string> &switch_names)
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
}  // namespace switchboard::grpc
