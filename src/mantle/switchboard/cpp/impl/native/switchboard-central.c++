// -*- c++ -*-
//==============================================================================
/// @file switchboard-central.c++
/// @brief Switchboard provider supporting only local switches
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-central.h++"
#include "central-switch.h++"
#include "switch-dependency.h++"
#include "platform/symbols.h++"
#include "platform/path.h++"
#include "status/exceptions.h++"
#include "settings/settings.h++"

namespace switchboard
{
    constexpr auto SETTING_SWITCH_CONFIG_FILES = "switch config files";

    constexpr auto SETTING_SWITCH_NAME = "name";
    constexpr auto SETTING_SWITCH_ACTIVE = "active";
    constexpr auto SETTING_SWITCH_SETTLED_STATE = "settled_state";
    constexpr auto SETTING_SWITCH_STATE = "state";
    constexpr auto SETTING_SWITCH_ATTRIBUTES = "attributes";
    constexpr auto SETTING_SWITCH_ERROR = "error";
    constexpr auto SETTING_SPEC_PRIMARY = "primary";
    constexpr auto SETTING_SPEC_ALIASES = "aliases";
    constexpr auto SETTING_SPEC_DEPENDENCIES = "dependencies";
    constexpr auto SETTING_SPEC_INTERCEPTORS = "interceptors";
    constexpr auto SETTING_SPEC_LOCALIZATIONS = "localizations";
    constexpr auto SETTING_LOC_DESCRIPTION = "description";
    constexpr auto SETTING_LOC_STATE_TEXTS = "state texts";
    constexpr auto SETTING_LOC_ACTIVATE_TEXT = "activate text";
    constexpr auto SETTING_LOC_DEACTIVATE_TEXT = "deactivate text";
    constexpr auto SETTING_DEP_PREDECESSOR = "predecessor";
    constexpr auto SETTING_DEP_TRIGGERS = "trigger_states";
    constexpr auto SETTING_DEP_AUTOMATIC = "automatic";
    constexpr auto SETTING_DEP_DIRECTION = "polarity";
    constexpr auto SETTING_DEP_INVERTED = "inverted";
    constexpr auto SETTING_DEP_HARD = "hard";
    constexpr auto SETTING_DEP_SUFFICIENT = "sufficient";

    Central::Central()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    void Central::initialize()
    {
        Super::initialize();
        this->load_default_switches();
    }

    bool Central::available() const
    {
        return true;
    }

    bool Central::wait_ready() const
    {
        return this->available();
    }

    std::pair<SwitchRef, bool> Central::add_switch(
        const SwitchName &switch_name,
        bool active)
    {
        const auto &[sw, inserted] = this->find_or_insert<CentralSwitch>(
            switch_name,
            this->shared_from_this());

        if (inserted)
        {
            sw->set_spec({});
            sw->status()->active = active;
            logf_info("Created switch: %s", sw->name());
            sw->notify_status();

            for (const auto &[key, data] : this->interceptor_factory_map)
            {
                if (data.switch_selection.matches(switch_name))
                {
                    sw->add_interceptor(data.interceptor);
                }
            }
        }

        return {sw, inserted};
    }

    bool Central::remove_switch(
        const SwitchName &name,
        bool propagate)
    {
        std::scoped_lock lck(this->switches_mutex);
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

        signal_status.clear(name);
        signal_spec.clear(name);
        return found;
    }

    bool Central::clear_switches(
        bool reload)
    {
        std::scoped_lock lck(this->switches_mutex);
        SwitchMap old_switches(std::move(this->switches));

        if (reload)
        {
            this->load_default_switches(
                true,           // replace_specifications
                true);          // replace_statuses
        }

        bool changes = false;
        for (const auto &[name, old_sw]: old_switches)
        {
            auto it = this->switches.find(name);
            if (it == this->switches.end())
            {
                signal_status.clear(name);
                signal_spec.clear(name);
                changes = true;
            }
            else if (!changes)
            {
                changes = (*it->second != *old_sw);
            }
        }

        return changes;
    }

    void Central::load_default_switches(
        bool replace_specifications,
        bool replace_statuses)
    {
        for (const auto &filename : core::settings->get(SETTING_SWITCH_CONFIG_FILES).get_valuelist())
        {
            try
            {
                logf_debug("Loading switches from: %s", filename);
                this->load(filename.as_string(),
                           replace_specifications,
                           replace_statuses);
            }
            catch (const std::exception &e)
            {
                logf_warning("Failed to load switches from %r: %s", filename, e);
            }
        }
    }

    uint Central::import_switches(const core::types::KeyValueMap &declarations,
                                  bool replace_specifications,
                                  bool replace_statuses)
    {
        uint count = 0;
        for (const auto &[name, declaration] : declarations)
        {
            count += this->import_switch(name,
                                         declaration.get_kvmap(),
                                         replace_specifications,
                                         replace_statuses);
        }
        return count;
    }

    core::types::KeyValueMap Central::export_switches(
        const std::optional<SwitchSelection> &selection,
        bool include_specifications,
        bool include_statuses) const
    {
        SwitchMap switches =
            !selection
                ? this->switches
            : selection->is_regex
                ? this->find_regex_matches(selection->patterns)
                : this->find_glob_matches(selection->patterns);

        core::types::KeyValueMap declarations;
        for (const auto &[name, sw] : switches)
        {
            declarations.insert_or_assign(
                name,
                this->export_switch(
                    sw,
                    include_specifications,
                    include_statuses));
        }
        return declarations;
    }

    bool Central::import_switch(
        const std::string &name,
        const core::types::KeyValueMap &declaration,
        bool replace_specification,
        bool replace_status)
    {
        bool default_active = declaration.get(SETTING_SWITCH_ACTIVE).as_bool();
        auto [sw, inserted] = this->add_switch(name, default_active);

        if (inserted || replace_specification)
        {
            this->import_spec(sw, declaration);
        }

        if (inserted || replace_status)
        {
            this->import_status(sw, declaration);
        }

        return inserted;
    }

    void Central::import_spec(
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

        for (const auto &[language, decl] : spec_map.get(SETTING_SPEC_LOCALIZATIONS).get_kvmap())
        {
            spec.localizations.emplace(
                language,
                This::import_localization(decl.get_kvmap()));
        }

        for (const auto &[predecessor, decl] : spec_map.get(SETTING_SPEC_DEPENDENCIES).get_kvmap())
        {
            spec.dependencies.emplace(
                predecessor,
                This::import_dependency(sw, predecessor, decl.get_kvmap()));
        }

        sw->set_spec(spec);
    }

    Localization Central::import_localization(const core::types::KeyValueMap &localization_map)
    {
        Localization localization;
        localization.description =
            localization_map
                .get(SETTING_LOC_DESCRIPTION)
                .as_string();

        localization.activate_text =
            localization_map
                .get(SETTING_LOC_ACTIVATE_TEXT)
                .as_string();

        localization.deactivate_text =
            localization_map
                .get(SETTING_LOC_DEACTIVATE_TEXT)
                .as_string();

        for (const auto &[key, value] :
             localization_map.get(SETTING_LOC_STATE_TEXTS).get_kvmap())
        {
            localization.state_texts.emplace(
                core::str::convert_to<State>(key, STATE_UNSET),
                value.to_string());
        }
        return localization;
    }

    DependencyRef Central::import_dependency(
        const SwitchRef &sw,
        const std::string &predecessor_name,
        const core::types::KeyValueMap &dep_map)
    {
        StateSet trigger_states;
        if (const auto &trigger_state_names = dep_map.get(SETTING_DEP_TRIGGERS).get_valuelist_ptr())
        {
            logf_trace("--- Switch %r trigger states: %s", sw->name(), *trigger_state_names);
            for (const core::types::Value &value : *trigger_state_names)
            {
                if (auto state = core::str::try_convert_to<State>(value.as_string()))
                {
                    trigger_states.insert(state.value());
                }
            }
            logf_trace("--- Switch %r trigger states: %s", sw->name(), trigger_states);
        }
        else if (const auto &automatic = dep_map.get(SETTING_DEP_AUTOMATIC))
        {
            trigger_states = automatic.as_bool() ? SETTLED_STATES : StateSet();
        }
        else
        {
            trigger_states = SETTLED_STATES;
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
                                         trigger_states,
                                         dir,
                                         hard,
                                         sufficient);
    }

    void Central::import_status(
        const SwitchRef &sw,
        const core::types::KeyValueMap &status)
    {
        const auto &attributes = status.get(SETTING_SWITCH_ATTRIBUTES).get_kvmap();

        if (const core::types::Value &error_spec = status.get(SETTING_SWITCH_ERROR))
        {
            sw->set_error(
                std::make_shared<core::status::Error>(error_spec.get_kvmap()),
                attributes);
        }
        else if (State state = status.get(SETTING_SWITCH_STATE).convert_to<State>())
        {
            sw->set_target(state, {}, attributes);
        }
        else
        {
            sw->set_attributes(attributes);
        }
    }

    bool Central::add_interceptor(
        const InterceptorRef &interceptor,
        const SwitchSelection &switch_selection,
        bool immediate,
        bool future)
    {
        bool inserted = false;
        if (future)
        {
            auto [it, added] = this->interceptor_factory_map.insert_or_assign(
                interceptor->name(),
                InterceptorFactoryData({
                    .interceptor = interceptor,
                    .switch_selection = switch_selection,
                    .immediate = immediate,
                }));

            inserted |= added;
        }

        for (const auto &[switch_name, sw] : this->get_switches())
        {
            if (switch_selection.matches(switch_name))
            {
                inserted |= sw->add_interceptor(interceptor, immediate);
            }
        }

        return inserted;
    }

    bool Central::remove_interceptor(
        const InterceptorName &interceptor_name,
        const std::optional<SwitchSelection> &switch_selection)
    {
        bool removed = false;
        for (const auto &[switch_name, sw] : this->get_switches())
        {
            if (!switch_selection || switch_selection->matches(switch_name))
            {
                removed |= sw->remove_interceptor(interceptor_name);
            }
        }
        return removed;
    }

    SwitchMap Central::find_regex_matches(
        const std::vector<std::string> &patterns) const
    {
        SwitchMap matches;

        std::vector<std::regex> rx_patterns;
        rx_patterns.reserve(patterns.size());
        for (const std::string &pattern : patterns)
        {
            rx_patterns.emplace_back(pattern);
        }

        for (const auto &[name, sw] : this->switches)
        {
            for (const std::regex &rx : rx_patterns)
            {
                if (std::regex_match(name, rx))
                {
                    matches.try_emplace(name, sw);
                    break;
                }
            }
        }
        return matches;
    }

    SwitchMap Central::find_glob_matches(
        const std::vector<std::string> &patterns) const
    {
        SwitchMap matches;
        for (const auto &[name, sw] : this->switches)
        {
            for (const std::string &pattern : patterns)
            {
                if (core::platform::path->filename_match(pattern, name, true))
                {
                    matches.try_emplace(name, sw);
                    break;
                }
            }
        }
        return matches;
    }

    core::types::KeyValueMap Central::export_switch(
        const SwitchRef &sw,
        bool include_specification,
        bool include_status)
    {
        core::types::TaggedValueList tvlist;
        if (include_specification)
        {
            sw->spec()->to_tvlist(&tvlist);
        }
        if (include_status)
        {
            This::export_status(sw, &tvlist);
        }
        return tvlist.as_kvmap();
    }

    void Central::export_status(
        const SwitchRef &sw,
        core::types::TaggedValueList *tvlist)
    {
        tvlist->append(
            SETTING_SWITCH_ACTIVE,
            sw->active());

        tvlist->append(
            SETTING_SWITCH_STATE,
            core::str::convert_from(sw->settled_state()));

        tvlist->append(
            SETTING_SWITCH_ATTRIBUTES,
            sw->attributes());

        if (sw->error() && *sw->error())
        {
            tvlist->append(
                SETTING_SWITCH_ERROR,
                sw->error()->as_kvmap());
        }
    }

}  // namespace switchboard
