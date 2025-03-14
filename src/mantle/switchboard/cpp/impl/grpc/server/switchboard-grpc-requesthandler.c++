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

    ::grpc::Status RequestHandler::get_switches(
        ::grpc::ServerContext *context,
        const ::google::protobuf::Empty *request,
        cc::switchboard::SwitchMap *reply)
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

    ::grpc::Status RequestHandler::get_switch(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::SwitchInfo *reply)
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

    ::grpc::Status RequestHandler::add_switch(
        ::grpc::ServerContext *context,
        const cc::switchboard::AddSwitchRequest *request,
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

    ::grpc::Status RequestHandler::remove_switch(
        ::grpc::ServerContext *context,
        const cc::switchboard::RemoveSwitchRequest *request,
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

    ::grpc::Status RequestHandler::set_specification(
        ::grpc::ServerContext *context,
        const cc::switchboard::SetSpecificationRequest *request,
        ::google::protobuf::BoolValue *reply)
    {
        try
        {
            if (auto sw = this->provider->get_switch(request->switch_name()))
            {
                const cc::switchboard::Specification &spec = request->spec();
                sw->update_spec(
                    spec.has_is_primary() ? spec.is_primary() : std::optional<bool>(),
                    protobuf::decoded<DescriptionMap>(spec.descriptions()),
                    request->replace_descriptions(),
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

    ::grpc::Status RequestHandler::get_specifications(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifiers *request,
        cc::switchboard::SpecificationMap *reply)
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

    ::grpc::Status RequestHandler::add_dependency(
        ::grpc::ServerContext *context,
        const cc::switchboard::AddDependencyRequest *request,
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

    ::grpc::Status RequestHandler::remove_dependency(
        ::grpc::ServerContext *context,
        const cc::switchboard::RemoveDependencyRequest *request,
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

    ::grpc::Status RequestHandler::get_dependencies(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::DependencyMap *reply)
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

    ::grpc::Status RequestHandler::get_ancestors(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::SwitchIdentifiers *reply)
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

    ::grpc::Status RequestHandler::get_descendents(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::SwitchIdentifiers *reply)
    {
        try
        {
            SwitchRef sw = this->provider->get_switch(request->switch_name(), true);
            protobuf::encode(sw->get_descendents(), reply);
            return ::grpc::Status::OK;
        }
        catch (...)
        {
            return this->failure(std::current_exception(), *request, context->peer());
        }
    }

    ::grpc::Status RequestHandler::add_interceptor(
        ::grpc::ServerContext *context,
        const cc::switchboard::AddInterceptorRequest *request,
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

    ::grpc::Status RequestHandler::remove_interceptor(
        ::grpc::ServerContext *context,
        const cc::switchboard::RemoveInterceptorRequest *request,
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

    ::grpc::Status RequestHandler::get_interceptors(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::InterceptorMap *reply)
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

    ::grpc::Status RequestHandler::invoke_interceptor(
        ::grpc::ServerContext *context,
        const cc::switchboard::InterceptorInvocation *request,
        cc::switchboard::InterceptorResult *reply)
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

    ::grpc::Status RequestHandler::intercept(
        ::grpc::ServerContext *context,
        InterceptorStream *stream)
    {
        return ::grpc::Status::OK;
    }

    ::grpc::Status RequestHandler::set_target(
        ::grpc::ServerContext *context,
        const cc::switchboard::SetTargetRequest *request,
        cc::switchboard::SetTargetResponse *reply)
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
                request->trigger_descendents(),
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

    ::grpc::Status RequestHandler::set_attributes(
        ::grpc::ServerContext *context,
        const cc::switchboard::SetAttributesRequest *request,
        cc::switchboard::SetAttributesResponse *reply)
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

    ::grpc::Status RequestHandler::get_statuses(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifiers *request,
        cc::switchboard::StatusMap *reply)
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

    ::grpc::Status RequestHandler::get_culprits(
        ::grpc::ServerContext *context,
        const cc::switchboard::CulpritsQuery *request,
        cc::switchboard::StatusMap *reply)
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

    ::grpc::Status RequestHandler::get_errors(
        ::grpc::ServerContext *context,
        const cc::switchboard::SwitchIdentifier *request,
        cc::switchboard::ErrorMap *reply)
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

    ::grpc::Status RequestHandler::watch(
        ::grpc::ServerContext *context,
        const cc::signal::Filter *request,
        ::grpc::ServerWriter<cc::switchboard::Signal> *writer)
    {
        return this->stream_signals<cc::switchboard::Signal, SignalQueue>(context, request, writer);
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
