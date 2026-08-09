// -*- c++ -*-
//==============================================================================
/// @file remote-dds-switch.c++
/// @brief Switch managed through remote DDS service
/// @author Tor Slettnes
//==============================================================================

#include "remote-dds-switch.h++"
#include "switchboard-dds-proxy.h++"
#include "translate-idl-switchboard.h++"
#include "translate-idl-variant.h++"
#include "translate-idl-inline.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace cc::platform::switchboard::dds
{
    std::shared_ptr<Client> RemoteSwitch::client() const
    {
        if (auto proxy = std::dynamic_pointer_cast<Proxy>(this->provider()))
        {
            return proxy->client();
        }
        else
        {
            throw core::exception::Unavailable("Switchboard proxy is not availble", this->name());
        }
    }

    bool RemoteSwitch::add_dependency(
        const DependencyRef& dependency,
        bool allow_update,
        bool reevaluate)
    {
        assertf(dependency, "Cannot add empty dependency reference");

        CC::Switchboard::AddDependencyRequest req;
        req.switch_name(this->name());
        idl::encode(dependency, &req.dependency());
        req.allow_update(allow_update);
        req.reevaluate(reevaluate);
        return this->client()->add_dependency(req);
    }

    bool RemoteSwitch::remove_dependency(
        SwitchName predecessor_name,
        bool reevaluate)
    {
        CC::Switchboard::RemoveDependencyRequest req;
        req.switch_name(this->name());
        req.predecessor_name(predecessor_name);
        req.reevaluate(reevaluate);
        return this->client()->remove_dependency(req);
    }

    bool RemoteSwitch::add_interceptor(
        const InterceptorRef& interceptor,
        bool immediate)
    {
        return this->provider()->add_interceptor(
            interceptor,   // interceptor
            this->name(),  // switch_selection
            immediate,     // immediate
            false);        // future
    }

    bool RemoteSwitch::remove_interceptor(
        const InterceptorName& name)
    {
        return this->provider()->remove_interceptor(
            name,           // name
            this->name());  // switch_selection
    }

    void RemoteSwitch::update_spec(
        const std::optional<bool>& primary,
        const SwitchAliases& aliases,
        bool replace_aliases,
        const LocalizationMap& localizations,
        bool replace_localizations,
        const DependencyMap& dependencies,
        bool replace_dependencies,
        const InterceptorMap& interceptors,
        bool replace_interceptors,
        bool update_state)
    {
        CC::Switchboard::SetSpecificationRequest req;
        CC::Switchboard::Specification& spec = req.spec();
        spec.switch_name(this->name());
        if (primary)
        {
            spec.is_primary(*primary);
        }
        idl::encode(aliases, &spec.aliases());
        idl::encode(localizations, &spec.localizations());
        idl::encode(dependencies, &spec.dependencies());
        idl::encode(interceptors, &spec.interceptors());

        req.replace_aliases(replace_aliases);
        req.replace_localizations(replace_localizations);
        req.replace_dependencies(replace_dependencies);
        req.replace_interceptors(replace_interceptors);
        req.update_state(update_state);

        this->client()->set_specification(req);
    }

    bool RemoteSwitch::set_target(
        State target_state,
        const core::status::Error::ptr& error,
        const core::types::KeyValueMap& attributes,
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

        CC::Switchboard::SetTargetRequest req;
        req.switch_name(this->name());
        req.target_state(idl::encoded<CC::Switchboard::State>(target_state));
        if (error)
        {
            req.error(idl::encoded<CC::Status::Error>(*error));
        }
        idl::encode(attributes, &req.attributes());
        req.clear_existing(clear_existing);
        req.invoke_interceptors(
            idl::encoded<CC::Switchboard::InvocationStyle>(invoke_interceptors));
        req.cascade_descendants(
            idl::encoded<CC::Switchboard::CascadeStyle>(cascade_descendants));
        req.reenter(reenter);
        idl::encode(on_cancel, &req.on_cancel());
        idl::encode(on_error, &req.on_error());
        return this->client()->set_target(req);
    }

    core::types::KeyValueMap RemoteSwitch::get_attributes(
        bool inherit) const
    {
        CC::Switchboard::GetAttributesRequest req;
        req.switch_name(this->name());
        req.inherit(inherit);
        return idl::decoded<core::types::KeyValueMap>(
            this->client()->get_attributes(req));
    }

    bool RemoteSwitch::set_attributes(
        const core::types::KeyValueMap& attributes,
        bool clear_existing)
    {
        CC::Switchboard::SetAttributesRequest req;
        req.switch_name(this->name());
        idl::encode(attributes, &req.attributes());
        req.clear_existing(clear_existing);
        return this->client()->set_attributes(req);
    }
}  // namespace cc::platform::switchboard::dds
