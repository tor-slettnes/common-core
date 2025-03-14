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
    constexpr auto SETTING_SPEC_DESCRIPTION = "description";
    constexpr auto SETTING_SPEC_PRIMARY = "primary";
    constexpr auto SETTING_SPEC_DEPENDENCIES = "dependencies";
    constexpr auto SETTING_SPEC_INTERCEPTORS = "interceptors";
    constexpr auto SETTING_SPEC_DESCRIPTION_TEXT = "text";
    constexpr auto SETTING_SPEC_STATE_TEXTS = "state_texts";
    constexpr auto SETTING_SPEC_TARGET_TEXTS = "target_texts";
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
        uint count = 0;

        for (const auto &[name, v] : store)
        {
            if (const core::types::KeyValueMap::ptr &kvmap = v.get_kvmap())
            {
                auto [sw, inserted] = this->add_switch(name);

                sw->set_spec(this->import_spec(sw, kvmap));

                auto attributes = kvmap->get(SETTING_SWITCH_ATTRIBUTES).as_kvmap();
                if (auto active = kvmap->get(SETTING_SWITCH_ACTIVE))
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

                count++;
            }
        }
        logf_debug("Added %r switches from %r", count, filename);
        return count;
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

    Specification Provider::import_spec(
        const SwitchRef &sw,
        const core::types::KeyValueMap::ptr &spec_map)
    {
        Specification spec;
        spec.primary = spec_map->get(SETTING_SPEC_PRIMARY).as_bool();

        if (const auto &kvmap = spec_map->get(SETTING_SPEC_DESCRIPTION).get_kvmap())
        {
            for (const auto &[language, value] : *kvmap)
            {
                if (const auto &localized_texts = value.get_kvmap())
                {
                    spec.descriptions.emplace(
                        language,
                        Provider::import_desc(localized_texts));
                }
            }
        }

        if (const auto &kvmap = spec_map->get(SETTING_SPEC_DEPENDENCIES).get_kvmap())
        {
            for (const auto &[key, value] : *kvmap)
            {
                if (const auto &dep_specs = value.get_kvmap())
                {
                    spec.dependencies.emplace(
                        key,
                        Provider::import_dependency(sw, key, dep_specs));
                }
            }
        }

        return spec;
    }

    Description Provider::import_desc(const core::types::KeyValueMap::ptr &desc_map)
    {
        Description desc;
        desc.text = desc_map->get(SETTING_SPEC_DESCRIPTION_TEXT).as_string();

        if (const auto &kvmap = desc_map->get(SETTING_SPEC_STATE_TEXTS).get_kvmap())
        {
            for (const auto &[key, value] : *kvmap)
            {
                desc.state_texts.emplace(
                    core::str::convert_to<State>(key, STATE_UNSET),
                    value.to_string());
            }
        }

        if (const auto &kvmap = desc_map->get(SETTING_SPEC_TARGET_TEXTS).get_kvmap())
        {
            for (const auto &[key, value] : *kvmap)
            {
                desc.target_texts.emplace(
                    core::str::convert_to<bool>(key, false),
                    value.to_string());
            }
        }
        return desc;
    }

    DependencyRef Provider::import_dependency(
        const SwitchRef &sw,
        const std::string &predecessor_name,
        const core::types::KeyValueMap::ptr &dep_map)
    {
        StateMask mask = 0;
        if (const auto &trigger_states = dep_map->get(SETTING_DEP_TRIGGERS).get_valuelist())
        {
            logf_trace("--- Switch %r trigger states: %s", sw->name(), *trigger_states);
            for (const core::types::Value &value : *trigger_states)
            {
                auto state = core::str::convert_to<State>(value.as_string(), STATE_UNSET);
                mask |= static_cast<StateMask>(state);
            }
            logf_trace("--- Switch %r trigger mask: %s", sw->name(), mask);
        }
        else if (bool automatic = dep_map->get(SETTING_DEP_AUTOMATIC).as_bool())
        {
            mask = SETTLED_STATES;
        }

        DependencyPolarity dir = DependencyPolarity::POSITIVE;
        if (const core::types::Value &polarity = dep_map->get(SETTING_DEP_DIRECTION))
        {
            dir = core::str::convert_to<DependencyPolarity>(polarity.as_string(), dir);
        }
        else if (const core::types::Value &inverted = dep_map->get(SETTING_DEP_INVERTED))
        {
            if (inverted.as_bool())
            {
                dir = DependencyPolarity::NEGATIVE;
            }
        }

        bool hard = dep_map->get(SETTING_DEP_HARD).as_bool();
        bool sufficient = dep_map->get(SETTING_DEP_SUFFICIENT).as_bool();

        return Dependency::create_shared(sw->provider(),
                                         predecessor_name,
                                         mask,
                                         dir,
                                         hard,
                                         sufficient);
    }

}  // namespace switchboard
