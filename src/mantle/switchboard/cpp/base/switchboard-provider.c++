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
    constexpr auto SETTING_SPEC_ALIASES = "aliases";
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

        if (auto switches = store.get(SETTINGS_SECTION_SWITCHES).get_valuelist_ptr())
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

    SwitchMap::const_iterator Provider::begin() const
    {
        return this->switches.begin();
    }

    SwitchMap::const_iterator Provider::end() const
    {
        return this->switches.end();
    }

    SwitchMap::iterator Provider::begin()
    {
        return this->switches.begin();
    }

    SwitchMap::iterator Provider::end()
    {
        return this->switches.end();
    }

    SwitchMap::const_iterator Provider::find(
        const SwitchName &name) const
    {
        if (auto it = this->switches.find(name);
            it != this->end())
        {
            return it;
        }

        for (auto it = this->begin(); it != this->end(); it++)
        {
            if (it->second->aliases().count(name))
            {
                return it;
            }
        }

        return this->end();
    }

    SwitchMap::iterator Provider::find(
        const SwitchName &name)
    {
        if (auto it = this->switches.find(name);
            it != this->switches.end())
        {
            return it;
        }

        for (auto it = this->begin(); it != this->end(); it++)
        {
            if (it->second->aliases().count(name))
            {
                return it;
            }
        }

        return this->end();
    }

    SwitchMap Provider::get_switches() const
    {
        return this->switches;
    }

    SwitchRef Provider::get_or_add_switch(const SwitchName &name)
    {
        if (SwitchRef sw = this->get_switch(name, false))
        {
            return sw;
        }
        else
        {
            return this->add_switch(name).first;
        }
    }

    SwitchRef Provider::get_switch(
        const SwitchName &name,
        bool required) const
    {
        if (auto it = this->find(name); it != this->end())
        {
            return it->second;
        }
        else if (required)
        {
            throwf(core::exception::NotFound,
                   ("Switch not found: %s", name),
                   name);
        }
        else
        {
            return {};
        }
    }

    bool Provider::remove_switch(
        const SwitchName &name,
        bool propagate)
    {
        auto it = this->find(name);
        bool found = it != this->end();

        if (found)
        {
            this->switches.erase(it);
            logf_info("Removed switch: %r", name);
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
            if (const core::types::Value &name = switch_info.get(SETTING_SPEC_NAME))
            {
                this->load_switch(name.as_string(), switch_info.get_kvmap());
                count += 1;
            }
        }
        return count;
    }

    void Provider::load_switch(const std::string &name,
                               const core::types::KeyValueMap &spec)
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
                           true,              // trigger_descendants
                           false);            // reevaluate
        }
        else
        {
            sw->set_auto(attributes,  // attributes
                         false,       // clear_existing
                         false,       // invoke_interceptors
                         true,        // trigger_descendants
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

        std::vector<SwitchName> aliases = spec_map
                                              .get(SETTING_SPEC_ALIASES)
                                              .get_valuelist()
                                              .filter_by_type<std::string>();

        spec.aliases.insert(aliases.begin(), aliases.end());

        for (const auto &localization : spec_map.get(SETTING_SPEC_LOCALIZATIONS).get_valuelist())
        {
            if (const auto &language_value = localization.get(SETTING_LOC_LANGUAGE))
            {
                spec.localizations.emplace(
                    language_value.as_string(),
                    This::import_localization(localization.get_kvmap()));
            }
            else
            {
                logf_notice("Ignoring switch %r localization without \"language\" key: %s",
                            sw->name(),
                            localization);
            }
        }

        for (const auto &dependency : spec_map.get(SETTING_SPEC_DEPENDENCIES).get_valuelist())
        {
            if (const auto &predecessor_value = dependency.get(SETTING_DEP_PREDECESSOR))
            {
                std::string predecessor = predecessor_value.as_string();
                spec.dependencies.emplace(
                    predecessor,
                    This::import_dependency(sw, predecessor, dependency.get_kvmap()));
            }
            else
            {
                logf_notice("Ignoring switch %r dependency without \"predecessor\" key: %s",
                            sw->name(),
                            dependency);
            }
        }

        return spec;
    }

    Localization Provider::import_localization(const core::types::KeyValueMap &localization_map)
    {
        Localization localization;
        localization.description = localization_map.get(SETTING_LOC_DESCRIPTION).as_string();

        for (const auto &[key, value] : localization_map.get(SETTING_LOC_STATE_TEXTS).get_kvmap())
        {
            localization.state_texts.emplace(
                core::str::convert_to<State>(key, STATE_UNSET),
                value.to_string());
        }

        for (const auto &[key, value] : localization_map.get(SETTING_LOC_TARGET_TEXTS).get_kvmap())
        {
            localization.target_texts.emplace(
                core::str::convert_to<bool>(key, false),
                value.to_string());
        }
        return localization;
    }

    DependencyRef Provider::import_dependency(
        const SwitchRef &sw,
        const std::string &predecessor_name,
        const core::types::KeyValueMap &dep_map)
    {
        StateMask mask = 0;
        if (const auto &trigger_states = dep_map.get(SETTING_DEP_TRIGGERS).get_valuelist_ptr())
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
