// -*- c++ -*-
//==============================================================================
/// @file remote-grpc-switch.c++
/// @brief Switch controlled via a remote gRPC service
/// @author Tor Slettnes
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

    std::shared_ptr<Proxy> RemoteSwitch::proxy() const
    {
        if (auto proxy = std::dynamic_pointer_cast<Proxy>(this->provider()))
        {
            return proxy;
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

        ::cc::platform::switchboard::protobuf::AddDependencyRequest req;
        req.set_switch_name(this->name());
        req.set_predecessor_name(dependency->predecessor_name());
        cc::protobuf::encode(dependency, req.mutable_dependency());
        req.set_allow_update(allow_update);
        req.set_reevaluate(reevaluate);
        return this->proxy()->call_check(&Proxy::Stub::AddDependency, req).value();
    }

    bool RemoteSwitch::remove_dependency(
        SwitchName predecessor_name,
        bool reevaluate)
    {
        ::cc::platform::switchboard::protobuf::RemoveDependencyRequest req;
        req.set_switch_name(this->name());
        req.set_predecessor_name(predecessor_name);
        req.set_reevaluate(reevaluate);
        return this->proxy()->call_check(&Proxy::Stub::RemoveDependency, req).value();
    }

    bool RemoteSwitch::add_interceptor(
        const InterceptorRef &interceptor,
        bool immediate)
    {
        return this->provider()->add_interceptor(
            interceptor,   // interceptor
            this->name(),  // switch_selection
            immediate,     // immediate
            false);        // future
    }

    bool RemoteSwitch::remove_interceptor(
        const InterceptorName &name)
    {
        return this->provider()->remove_interceptor(
            name,           // name
            this->name());  // switch_selection
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
        ::cc::platform::switchboard::protobuf::SetSpecificationRequest req;
        req.set_switch_name(this->name());

        ::cc::platform::switchboard::protobuf::Specification *spec = req.mutable_spec();
        if (primary)
        {
            spec->set_is_primary(primary.value());
        }

        cc::protobuf::encode(aliases, spec->mutable_aliases());
        cc::protobuf::encode(localizations, spec->mutable_localizations());
        cc::protobuf::encode(dependencies, spec->mutable_dependencies());
        cc::protobuf::encode(interceptors, spec->mutable_interceptors());

        req.set_replace_aliases(replace_aliases);
        req.set_replace_localizations(replace_localizations);
        req.set_replace_dependencies(replace_dependencies);
        req.set_replace_interceptors(replace_interceptors);
        req.set_update_state(update_state);

        this->proxy()->call_check(&Proxy::Stub::SetSpecification, req);
    }

    bool RemoteSwitch::set_target(
        State target_state,
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        InvocationStyle invoke_interceptors,
        CascadeStyle cascade_descendants,
        bool reenter,
        ExceptionHandling on_cancel,
        ExceptionHandling on_error)
    {
        logf_debug("Setting %r target state %r, error %r, attributes %r",
                   this->name(),
                   target_state,
                   error,
                   attributes);

        ::cc::platform::switchboard::protobuf::SetTargetRequest req;
        req.set_switch_name(this->name());
        req.set_target_state(
            cc::protobuf::encoded<::cc::platform::switchboard::protobuf::State>(target_state));

        if (error)
        {
            cc::protobuf::encode_shared(error, req.mutable_error());
        }
        cc::protobuf::encode(attributes, req.mutable_attributes());
        req.set_clear_existing(clear_existing);

        req.set_invoke_interceptors(
            cc::protobuf::encoded<::cc::platform::switchboard::protobuf::InvocationStyle>(
                invoke_interceptors));

        req.set_cascade_descendants(
            cc::protobuf::encoded<::cc::platform::switchboard::protobuf::CascadeStyle>(
                cascade_descendants));

        req.set_reenter(reenter);

        req.set_on_cancel(
            cc::protobuf::encoded<::cc::platform::switchboard::protobuf::ExceptionHandling>(
                on_cancel));

        req.set_on_error(
            cc::protobuf::encoded<::cc::platform::switchboard::protobuf::ExceptionHandling>(
                on_error));
        return this->proxy()->call_check(&Proxy::Stub::SetTarget, req).updated();
    }

    core::types::KeyValueMap RemoteSwitch::get_attributes(
        bool inherit) const
    {
        ::cc::platform::switchboard::protobuf::GetAttributesRequest req;
        req.set_switch_name(this->name());
        req.set_inherit(inherit);

        return cc::protobuf::decoded<core::types::KeyValueMap>(
            this->proxy()->call_check(&Proxy::Stub::GetAttributes, req).attributes());
    }

    bool RemoteSwitch::set_attributes(
        const core::types::KeyValueMap &attributes,
        bool clear_existing)
    {
        ::cc::platform::switchboard::protobuf::SetAttributesRequest req;
        req.set_switch_name(this->name());
        cc::protobuf::encode(attributes, req.mutable_attributes());
        req.set_clear_existing(clear_existing);
        return this->proxy()->call_check(&Proxy::Stub::SetAttributes, req).updated();
    }

}  // namespace switchboard::grpc
