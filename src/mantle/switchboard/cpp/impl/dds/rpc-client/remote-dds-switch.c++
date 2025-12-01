// -*- c++ -*-
//==============================================================================
/// @file remote-dds-switch.c++
/// @brief Switch managed through remote DDS service
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "remote-dds-switch.h++"
#include "switchboard-dds-proxy.h++"
#include "translate-idl-switchboard.h++"
#include "translate-idl-variant.h++"
#include "translate-idl-inline.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace switchboard::dds
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
        const DependencyRef &dependency,
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
        const InterceptorRef &interceptor,
        bool immediate)
    {
        CC::Switchboard::AddInterceptorRequest req;
        req.switch_name(this->name());
        idl::encode(interceptor, &req.spec());
        req.immediate(immediate);
        return this->client()->add_interceptor(req);
    }

    bool RemoteSwitch::remove_interceptor(
        const InterceptorName &name)
    {
        CC::Switchboard::RemoveInterceptorRequest req;
        req.switch_name(this->name());
        req.interceptor_name(name);
        return this->client()->remove_interceptor(req);
    }

    void RemoteSwitch::update_spec(
        const std::optional<bool> &primary,
        const LocalizationMap &localizations,
        bool replace_localizations,
        const DependencyMap &dependencies,
        bool replace_dependencies,
        const InterceptorMap &interceptors,
        bool replace_interceptors,
        bool update_state)
    {
        CC::Switchboard::SetSpecificationRequest req;
        CC::Switchboard::Specification &spec = req.spec();
        spec.switch_name(this->name());
        if (primary)
        {
            spec.is_primary(*primary);
        }
        idl::encode(localizations, &spec.localizations());
        idl::encode(dependencies, &spec.dependencies());
        idl::encode(interceptors, &spec.interceptors());

        req.replace_localizations(replace_localizations);
        req.replace_dependencies(replace_dependencies);
        req.replace_interceptors(replace_interceptors);
        req.update_state(update_state);

        this->client()->set_specification(req);
    }

    bool RemoteSwitch::set_target(
        State target_state,
        const core::status::Error::ptr &error,
        const core::types::KeyValueMap &attributes,
        bool clear_existing,
        bool invoke_interceptors,
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

        CC::Switchboard::SetTargetRequest req;
        req.switch_name(this->name());
        req.target_state(idl::encoded<CC::Switchboard::State>(target_state));
        if (error)
        {
            req.error(idl::encoded<CC::Status::Error>(*error));
        }
        idl::encode(attributes, &req.attributes());
        req.clear_existing(clear_existing);
        req.invoke_interceptors(invoke_interceptors);
        req.trigger_descendants(trigger_descendants);
        req.reevaluate(reevaluate);
        idl::encode(on_cancel, &req.on_cancel());
        idl::encode(on_error, &req.on_error());
        return this->client()->set_target(req);
    }

    bool RemoteSwitch::set_attributes(
        const core::types::KeyValueMap &attributes,
        bool clear_existing)
    {
        CC::Switchboard::SetAttributesRequest req;
        req.switch_name(this->name());
        idl::encode(attributes, &req.attributes());
        req.clear_existing(clear_existing);
        return this->client()->set_attributes(req);
    }
}  // namespace switchboard
