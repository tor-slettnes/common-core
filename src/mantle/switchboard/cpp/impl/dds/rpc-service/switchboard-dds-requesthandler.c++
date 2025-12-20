// -*- c++ -*-
//==============================================================================
/// @file switchboard-dds-requesthandler.c++
/// @brief Handle Switchboard RPC requests
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-dds-requesthandler.h++"
#include "translate-idl-switchboard.h++"
#include "translate-idl-variant.h++"
#include "translate-idl-inline.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

namespace switchboard::dds
{
    RequestHandler::RequestHandler(
        const std::shared_ptr<Provider> &provider)
        : Super(),
          provider(provider)
    {
        logf_debug("Switchboard DDS request handler");
    }

    CC::Switchboard::SwitchList RequestHandler::get_switches()
    {
        return idl::encoded<CC::Switchboard::SwitchList>(this->provider->get_switches());
    }

    CC::Switchboard::Switch RequestHandler::get_switch(const SwitchName &switch_name)
    {
        logf_debug("Received get_switch(%r)", switch_name);
        if (SwitchRef sw = this->provider->get_switch(switch_name))
        {
            return idl::encoded<CC::Switchboard::Switch>(*sw);
        }
        else
        {
            return {};
        }
    }

    bool RequestHandler::add_switch(
        const CC::Switchboard::AddSwitchRequest &req)
    {
        logf_debug("Add_switch(%r)", req.switch_name());

        auto [sw, inserted] = this->provider->add_switch(req.switch_name());
        return inserted;
    }

    bool RequestHandler::remove_switch(
        const CC::Switchboard::RemoveSwitchRequest &req)
    {
        return this->provider->remove_switch(
            req.switch_name(),
            req.propagate());
    }

    std::uint32_t RequestHandler::import_switches(
        const CC::Variant::ValueList &req)
    {
        return this->provider->import_switches(
            idl::decoded<core::types::ValueList>(req));
    }

    bool RequestHandler::set_specification(
        const CC::Switchboard::SetSpecificationRequest &req)
    {
        const CC::Switchboard::Specification &spec = req.spec();
        if (auto sw = this->provider->get_switch(spec.switch_name(), true))
        {
            sw->update_spec(
                idl::decode_optional(spec.is_primary()),
                idl::decoded<switchboard::SwitchAliases>(spec.aliases()),
                req.replace_aliases(),
                idl::decoded<LocalizationMap>(spec.localizations()),
                req.replace_localizations(),
                idl::decoded<DependencyMap>(spec.dependencies(), this->provider),
                req.replace_dependencies(),
                idl::decoded<InterceptorMap>(spec.interceptors()),
                req.replace_interceptors(),
                req.update_state());
            return true;
        }
        else
        {
            return false;
        }
    }

    bool RequestHandler::add_dependency(
        const CC::Switchboard::AddDependencyRequest &req)
    {
        SwitchRef sw = this->provider->get_switch(req.switch_name(), true);
        DependencyRef dep = idl::decoded<DependencyRef>(req.dependency(), this->provider);
        return sw->add_dependency(dep, req.allow_update(), req.reevaluate());
    }

    bool RequestHandler::remove_dependency(
        const CC::Switchboard::RemoveDependencyRequest &req)
    {
        if (auto sw = this->provider->get_switch(req.switch_name(), true))
        {
            return sw->remove_dependency(req.predecessor_name(), req.reevaluate());
        }
        else
        {
            return false;
        }
    }

    bool RequestHandler::add_interceptor(
        const CC::Switchboard::AddInterceptorRequest &req)
    {
        SwitchRef sw = this->provider->get_switch(req.switch_name(), true);
        InterceptorRef icept = idl::decoded<InterceptorRef>(
            req.spec(),
            [](SwitchRef sw, State state) {
                logf_info("Callback for switch=%r, state=%s", *sw, state);
            });

        return sw->add_interceptor(icept, req.immediate());
    }

    bool RequestHandler::remove_interceptor(
        const CC::Switchboard::RemoveInterceptorRequest &req)
    {
        SwitchRef sw = this->provider->get_switch(req.switch_name(), true);
        return sw->remove_interceptor(req.interceptor_name());
    }

    CC::Switchboard::InterceptorResult RequestHandler::invoke_interceptor(
        const CC::Switchboard::InterceptorInvocation &req)
    {
        CC::Switchboard::InterceptorResult result;

        SwitchRef sw = this->provider->get_switch(req.switch_name(), true);
        result.switch_name(sw->name());

        InterceptorRef icept = sw->get_interceptor(req.interceptor_name(), true);
        result.interceptor_name(icept->name());

        std::future<void> future = icept->invoke(sw, idl::decoded<State>(req.state()));

        try
        {
            future.wait();
        }
        catch (...)
        {
            result.error(
                idl::encoded<CC::Status::Error>(
                    *core::exception::map_to_error(std::current_exception())));
        }

        return result;
    }

    bool RequestHandler::set_target(
        const CC::Switchboard::SetTargetRequest &req)
    {
        if (SwitchRef sw = this->provider->get_switch(req.switch_name(), true))
        {
            core::status::Error::ptr error;
            if (auto error_data = req.error().get_ptr())
            {
                idl::decode_shared(*error_data, &error);
            }
            return sw->set_target(
                idl::decoded<switchboard::State>(req.target_state()),
                error,
                idl::decoded<core::types::KeyValueMap>(req.attributes()),
                req.clear_existing(),
                req.invoke_interceptors(),
                req.trigger_descendants(),
                req.reevaluate(),
                idl::decoded<switchboard::ExceptionHandling>(req.on_cancel()),
                idl::decoded<switchboard::ExceptionHandling>(req.on_error()));
        }
        else
        {
            return false;
        }
    }

    bool RequestHandler::set_attributes(
        const CC::Switchboard::SetAttributesRequest &req)
    {
        if (SwitchRef sw = this->provider->get_switch(req.switch_name(), true))
        {
            return sw->set_attributes(
                idl::decoded<core::types::KeyValueMap>(req.attributes()),
                req.clear_existing());
        }
        else
        {
            return false;
        }
    }

    CC::Switchboard::StatusList RequestHandler::get_culprits(
        const CC::Switchboard::CulpritsQuery &req)
    {
        std::vector<CC::Switchboard::Status> list;
        if (SwitchRef sw = this->provider->get_switch(req.switch_name(), true))
        {
            for (const auto &[sw, state] : sw->culprits(req.expected_active()))
            {
                idl::encode(sw->name(), *sw->status(), &list.emplace_back());
            }
        }

        CC::Switchboard::StatusList statuslist;
        statuslist.list(std::move(list));
        return statuslist;
    }

}  // namespace switchboard::dds
