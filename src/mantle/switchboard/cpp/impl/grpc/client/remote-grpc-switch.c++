// -*- c++ -*-
//==============================================================================
/// @file remote-grpc-switch.c++
/// @brief Switch controlled via a remote gRPC service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "remote-grpc-switch.h++"
#include "protobuf-switchboard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace switchboard::grpc
{
    //==========================================================================
    /// @class RemoteSwitch

    std::shared_ptr<RemoteSwitch::Client> RemoteSwitch::client() const
    {
        if (auto client = std::dynamic_pointer_cast<Client>(this->provider()))
        {
            return client;
        }
        else
        {
            throw core::exception::Unavailable("Switchboard proxy is not availble", this->name());
        }
    }

    bool RemoteSwitch::add_dependency(
        const DependencyRef &dependency,
        bool allow_update,
        bool reevaluate)
    {
        assertf(dependency, "Cannot add empty dependency reference");

        cc::platform::switchboard::protobuf::AddDependencyRequest req;
        req.set_switch_name(this->name());
        req.set_predecessor_name(dependency->predecessor_name());
        protobuf::encode(dependency, req.mutable_dependency());
        req.set_allow_update(allow_update);
        req.set_reevaluate(reevaluate);
        return this->client()->call_check(&Client::Stub::AddDependency, req).value();
    }

    bool RemoteSwitch::remove_dependency(
        SwitchName predecessor_name,
        bool reevaluate)
    {
        cc::platform::switchboard::protobuf::RemoveDependencyRequest req;
        req.set_switch_name(this->name());
        req.set_predecessor_name(predecessor_name);
        req.set_reevaluate(reevaluate);
        return this->client()->call_check(&Client::Stub::RemoveDependency, req).value();
    }

    bool RemoteSwitch::add_interceptor(
        const InterceptorRef &interceptor,
        bool immediate)
    {
        cc::platform::switchboard::protobuf::AddInterceptorRequest req;
        req.set_switch_name(this->name());
        req.set_interceptor_name(interceptor->name());
        protobuf::encode(interceptor, req.mutable_spec());
        req.set_immediate(immediate);
        return this->client()->call_check(&Client::Stub::AddInterceptor, req).value();
    }

    bool RemoteSwitch::remove_interceptor(
        const InterceptorName &id)
    {
        cc::platform::switchboard::protobuf::RemoveInterceptorRequest req;
        req.set_switch_name(this->name());
        req.set_interceptor_name(id);
        return this->client()->call_check(&Client::Stub::RemoveInterceptor, req).value();
    }

    void RemoteSwitch::update_spec(
        const std::optional<bool> &primary,
        const SwitchAliases &aliases,
        bool replace_aliases,
        const LocalizationMap &localizations,
        bool replace_localizations,
        const DependencyMap &dependencies,
        bool replace_dependencies,
        const InterceptorMap &interceptors,
        bool replace_interceptors,
        bool update_state)
    {
        cc::platform::switchboard::protobuf::SetSpecificationRequest req;
        req.set_switch_name(this->name());

        cc::platform::switchboard::protobuf::Specification *spec = req.mutable_spec();
        if (primary)
        {
            spec->set_is_primary(primary.value());
        }

        protobuf::encode(aliases, spec->mutable_aliases());
        protobuf::encode(localizations, spec->mutable_localizations());
        protobuf::encode(dependencies, spec->mutable_dependencies());
        protobuf::encode(interceptors, spec->mutable_interceptors());

        req.set_replace_aliases(replace_aliases);
        req.set_replace_localizations(replace_localizations);
        req.set_replace_dependencies(replace_dependencies);
        req.set_replace_interceptors(replace_interceptors);
        req.set_update_state(update_state);

        this->client()->call_check(&Client::Stub::SetSpecification, req);
    }

    bool RemoteSwitch::set_target(
        State target_state,
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool with_interceptors,
        bool trigger_descendants,
        bool reevaluate,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        logf_debug("Setting %r target state %r, error %r, attributes %r",
                   this->name(),
                   target_state,
                   error,
                   attributes);

        cc::platform::switchboard::protobuf::SetTargetRequest req;
        req.set_switch_name(this->name());
        req.set_target_state(protobuf::encoded<cc::platform::switchboard::protobuf::State>(target_state));
        protobuf::encode_shared(error, req.mutable_error());
        protobuf::encode(attributes, req.mutable_attributes());
        req.set_clear_existing(clear_existing);
        req.set_with_interceptors(with_interceptors);
        req.set_trigger_descendants(trigger_descendants);
        req.set_reevaluate(reevaluate);
        req.set_on_cancel(protobuf::encoded<cc::platform::switchboard::protobuf::ExceptionHandling>(on_cancel));
        req.set_on_error(protobuf::encoded<cc::platform::switchboard::protobuf::ExceptionHandling>(on_error));
        return this->client()->call_check(&Client::Stub::SetTarget, req).updated();
    }

    bool RemoteSwitch::set_attributes(
        const core::types::KeyValueMap &attributes,
        bool clear_existing)
    {
        cc::platform::switchboard::protobuf::SetAttributesRequest req;
        req.set_switch_name(this->name());
        protobuf::encode(attributes, req.mutable_attributes());
        req.set_clear_existing(clear_existing);
        return this->client()->call_check(&Client::Stub::SetAttributes, req).updated();
    }

}  // namespace switchboard::grpc
