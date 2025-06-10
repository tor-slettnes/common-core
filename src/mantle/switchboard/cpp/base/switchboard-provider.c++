// -*- c++ -*-
//==============================================================================
/// @file switchboard-provider.c++
/// @brief Abstract switchboard provider API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-provider.h++"
#include "settings/settingsstore.h++"
#include "status/exceptions.h++"

namespace switchboard
{
    constexpr auto SETTINGS_SECTION_SWITCHES = "switches";
    constexpr auto SETTING_SPEC_NAME = "name";
    constexpr auto SETTING_SPEC_PRIMARY = "primary";
    constexpr auto SETTING_SPEC_DEPENDENCIES = "dependencies";
    constexpr auto SETTING_SPEC_INTERCEPTORS = "interceptors";
    constexpr auto SETTING_SPEC_LOCALIZATIONS = "localizations";
    constexpr auto SETTING_LOC_LANGUAGE = "language";
    constexpr auto SETTING_LOC_DESCRIPTION = "description";
    constexpr auto SETTING_LOC_STATE_TEXTS = "state texts";
    constexpr auto SETTING_LOC_TARGET_TEXTS = "target texts";
    constexpr auto SETTING_DEP_PREDECESSOR = "predecessor";
    constexpr auto SETTING_DEP_TRIGGERS = "trigger_states";
    constexpr auto SETTING_DEP_AUTOMATIC = "automatic";
    constexpr auto SETTING_DEP_DIRECTION = "polarity";
    constexpr auto SETTING_DEP_INVERTED = "inverted";
    constexpr auto SETTING_DEP_HARD = "hard";
    constexpr auto SETTING_DEP_SUFFICIENT = "sufficient";
    constexpr auto SETTING_SWITCH_ACTIVE = "active";
    constexpr auto SETTING_SWITCH_ATTRIBUTES = "attributes";

    //==========================================================================
    // Provider

    Provider::Provider(const std::string &implementation)
        : implementation_(implementation)
    {
    }

    const std::string &Provider::implementation() const
    {
        return this->implementation_;
    }

    bool Provider::load(const fs::path &filename)
    {
        core::SettingsStore store(filename);

        if (auto switches = store.get(SETTINGS_SECTION_SWITCHES).get_valuelist())
        {
            uint count = this->load_switches(*switches);
            logf_info("Added %r switches from %r", count, filename);
            return (count > 0);
        }
        else
        {
            return false;
        }
    }

    SwitchMap Provider::get_switches() const
    {
        return this->switches;
    }

    SwitchRef Provider::get_or_add_switch(const SwitchName &name)
    {
        try
        {
            return this->switches.at(name);
        }
        catch (const std::out_of_range &)
        {
            return this->add_switch(name).first;
        }
    }

    SwitchRef Provider::get_switch(
        const SwitchName &name,
        bool required) const
    {
        try
        {
            return this->switches.at(name);
        }
        catch (const std::out_of_range &)
        {
            if (required)
            {
                throwf(core::exception::NotFound,
                       ("Switch not found: %s", name),
                       name);
            }
            return {};
        }
    }

    bool Provider::remove_switch(
        const SwitchName &name,
        bool propagate)
    {
        bool found = this->switches.erase(name);
        if (found)
        {
            for (const auto &[candidate, sw] : this->switches)
            {
                sw->remove_dependency(name, propagate);
            }
        }
        return found;
    }

    /// @brief
    ///     Remove an existing interceptor
    bool Provider::remove_interceptor(const SwitchName &switch_name,
                                      const InterceptorName &interceptor_name)
    {
        if (auto sw = this->get_switch(switch_name))
        {
            return sw->remove_interceptor(interceptor_name);
        }
        else
        {
            return false;
        }
    }

    uint Provider::load_switches(const core::types::ValueList &switches)
    {
        uint count = 0;
        for (const core::types::Value &switch_info : switches)
        {
            if (auto kvmap = switch_info.get_kvmap())
            {
                if (const core::types::Value &name = kvmap->get(SETTING_SPEC_NAME))
                {
                    this->load_switch(name.as_string(), *kvmap);
                    count += 1;
                }
            }
        }
        return count;
    }

    void Provider::load_switch(const std::string &name, core::types::KeyValueMap &spec)
    {
        auto [sw, inserted] = this->add_switch(name);
        sw->set_spec(this->import_spec(sw, spec));

        auto attributes = spec.get(SETTING_SWITCH_ATTRIBUTES).as_kvmap();
        if (auto active = spec.get(SETTING_SWITCH_ACTIVE))
        {
            sw->set_active(active.as_bool(),  // active
                           attributes,        // attributes
                           false,             // clear_existing
                           false,             // invoke_interceptors
                           true,              // trigger_descendents
                           false);            // reevaluate
        }
        else
        {
            sw->set_auto(attributes,  // attributes
                         false,       // clear_existing
                         false,       // invoke_interceptors
                         true,        // trigger_descendents
                         false);      // reevaluate
        }

        logf_debug("Loaded switch: %s", *sw);
    }

    Specification Provider::import_spec(
        const SwitchRef &sw,
        const core::types::KeyValueMap &spec_map)
    {
        Specification spec;
        spec.primary = spec_map.get(SETTING_SPEC_PRIMARY).as_bool();

        if (auto localizations = spec_map.get(SETTING_SPEC_LOCALIZATIONS).get_valuelist())
        {
            for (const auto &localization : *localizations)
            {
                if (const auto &language_value = localization.get(SETTING_LOC_LANGUAGE))
                {
                    spec.localizations.emplace(
                        language_value.as_string(),
                        This::import_localization(*localization.get_kvmap()));
                }
                else
                {
                    logf_notice("Ignoring switch %r localization without \"language\" key: %s",
                                sw->name(),
                                localization);
                }
            }
        }

        if (auto dependencies = spec_map.get(SETTING_SPEC_DEPENDENCIES).get_valuelist())
        {
            for (const auto &dependency : *dependencies)
            {
                if (const auto &predecessor_value = dependency.get(SETTING_DEP_PREDECESSOR))
                {
                    std::string predecessor = predecessor_value.as_string();
                    spec.dependencies.emplace(
                        predecessor,
                        This::import_dependency(sw, predecessor, *dependency.get_kvmap()));
                }
                else
                {
                    logf_notice("Ignoring switch %r dependency without \"predecessor\" key: %s",
                                sw->name(),
                                dependency);
                }
            }
        }

        return spec;
    }

    Localization Provider::import_localization(const core::types::KeyValueMap &localization_map)
    {
        Localization localization;
        localization.description = localization_map.get(SETTING_LOC_DESCRIPTION).as_string();

        if (const auto &kvmap = localization_map.get(SETTING_LOC_STATE_TEXTS).get_kvmap())
        {
            for (const auto &[key, value] : *kvmap)
            {
                localization.state_texts.emplace(
                    core::str::convert_to<State>(key, STATE_UNSET),
                    value.to_string());
            }
        }

        if (const auto &kvmap = localization_map.get(SETTING_LOC_TARGET_TEXTS).get_kvmap())
        {
            for (const auto &[key, value] : *kvmap)
            {
                localization.target_texts.emplace(
                    core::str::convert_to<bool>(key, false),
                    value.to_string());
            }
        }
        return localization;
    }

    DependencyRef Provider::import_dependency(
        const SwitchRef &sw,
        const std::string &predecessor_name,
        const core::types::KeyValueMap &dep_map)
    {
        StateMask mask = 0;
        if (const auto &trigger_states = dep_map.get(SETTING_DEP_TRIGGERS).get_valuelist())
        {
            logf_trace("--- Switch %r trigger states: %s", sw->name(), *trigger_states);
            for (const core::types::Value &value : *trigger_states)
            {
                auto state = core::str::convert_to<State>(value.as_string(), STATE_UNSET);
                mask |= static_cast<StateMask>(state);
            }
            logf_trace("--- Switch %r trigger mask: %s", sw->name(), mask);
        }
        else if (bool automatic = dep_map.get(SETTING_DEP_AUTOMATIC).as_bool())
        {
            mask = SETTLED_STATES;
        }

        DependencyPolarity dir = DependencyPolarity::POSITIVE;
        if (const core::types::Value &polarity = dep_map.get(SETTING_DEP_DIRECTION))
        {
            dir = core::str::convert_to<DependencyPolarity>(polarity.as_string(), dir);
        }
        else if (const core::types::Value &inverted = dep_map.get(SETTING_DEP_INVERTED))
        {
            if (inverted.as_bool())
            {
                dir = DependencyPolarity::NEGATIVE;
            }
        }

        bool hard = dep_map.get(SETTING_DEP_HARD).as_bool();
        bool sufficient = dep_map.get(SETTING_DEP_SUFFICIENT).as_bool();

        return Dependency::create_shared(sw->provider(),
                                         predecessor_name,
                                         mask,
                                         dir,
                                         hard,
                                         sufficient);
    }

    std::shared_ptr<Provider> provider;
}  // namespace switchboard
