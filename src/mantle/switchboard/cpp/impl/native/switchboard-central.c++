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
                true,   // replace_specifications
                true);  // replace_statuses
        }

        bool changes = false;
        for (const auto &[name, old_sw] : old_switches)
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

    bool Central::import_switch(
        const std::string &name,
        const core::types::KeyValueMap &declaration,
        bool replace_specification,
        bool replace_status)
    {
        bool default_active = declaration.get(SETTING_SWITCH_ACTIVE).as_bool();
        auto [sw, inserted] = this->add_switch(name, default_active);

        if (auto central_switch = std::dynamic_pointer_cast<CentralSwitch>(sw))
        {
            central_switch->import_spec(
                declaration,            // declaration
                replace_specification,  // replace_aliases
                replace_specification,  // replace_localizations
                replace_specification,  // replace_dependencies
                false);                 // replace_interceptors

            central_switch->import_status(
                declaration,      // declaration
                replace_status);  // replace_attributes
        }

        return inserted;
    }

    core::types::KeyValueMap Central::export_switch(
        const SwitchRef &sw,
        bool include_specification,
        bool include_status) const
    {
        core::types::TaggedValueList tvlist;
        if (auto central_switch = std::dynamic_pointer_cast<CentralSwitch>(sw))
        {
            if (include_specification)
            {
                central_switch->export_spec(&tvlist);
            }
            if (include_status)
            {
                central_switch->export_status(&tvlist);
            }
        }

        return tvlist.as_kvmap();
    }

}  // namespace switchboard
