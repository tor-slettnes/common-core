// -*- c++ -*-
//==============================================================================
/// @file switchboard-provider.c++
/// @brief Abstract switchboard provider API
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-provider.h++"
#include "settings/settingsstore.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"

namespace cc::platform::switchboard
{
    constexpr auto SETTINGS_SECTION_SWITCHES = "switches";

    //==========================================================================
    // Provider

    Provider::Provider(const std::string& implementation)
        : implementation_(implementation)
    {
    }

    const std::string& Provider::implementation() const
    {
        return this->implementation_;
    }

    bool Provider::load(const fs::path& filename,
                        bool replace_specifications,
                        bool replace_statuses,
                        InvocationStyle invoke_interceptors)
    {
        core::SettingsStore store(filename);
        uint count = this->import_switches(
            store,
            replace_specifications,
            replace_statuses,
            invoke_interceptors);
        logf_info("Added %r switches from %r", count, filename);
        return (count > 0);
    }

    bool Provider::save(const fs::path& filename,
                        bool include_specifications,
                        bool include_statuses)
    {
        core::SettingsStore store;
        store.update(this->export_switches(
            {},
            include_specifications,
            include_statuses));

        try
        {
            store.save_to(filename);
            return true;
        }
        catch (...)
        {
            logf_error(
                "Failed so save switches to %r: %s",
                filename,
                std::current_exception());
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
        const SwitchName& name) const
    {
        std::scoped_lock lck(const_cast<This*>(this)->switches_mutex);
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
        const SwitchName& name)
    {
        std::scoped_lock lck(this->switches_mutex);
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

    SwitchMap Provider::get_selected_switches(
        const SwitchSelection& selection) const
    {
        if (selection.patterns.empty())
        {
            return this->get_switches();
        }
        else
        {
            SwitchMap matches =
                selection.is_regex
                    ? this->find_regex_matches(selection.patterns)
                    : this->find_glob_matches(selection.patterns);

            if (selection.with_ancestors)
            {
                SwitchMap indirect_matches;
                for (const auto& [switch_name, sw] : matches)
                {
                    indirect_matches.try_emplace(switch_name, sw);
                    for (const auto& ancestor : sw->get_ancestors())
                    {
                        indirect_matches.try_emplace(ancestor->name(), ancestor);
                    }
                }
                matches.merge(std::move(indirect_matches));
            }

            return matches;
        }
    }

    SwitchRef Provider::get_or_add_switch(
        const SwitchName& name,
        bool active)
    {
        std::scoped_lock lck(this->switches_mutex);
        if (SwitchRef sw = this->get_switch(name, false))
        {
            return sw;
        }
        else
        {
            return this->add_switch(name, active).first;
        }
    }

    SwitchRef Provider::get_switch(
        const SwitchName& name,
        bool required) const
    {
        std::scoped_lock lck(const_cast<This*>(this)->switches_mutex);
        if (auto it = this->find(name); it != this->end())
        {
            return it->second;
        }
        else if (required)
        {
            throwf_args(core::exception::NotFound,
                        ("Switch not found: %s", name),
                        name);
        }
        else
        {
            return {};
        }
    }

    SwitchMap Provider::find_regex_matches(
        const std::vector<std::string>& patterns) const
    {
        SwitchMap matches;

        std::vector<std::regex> rx_patterns;
        rx_patterns.reserve(patterns.size());
        for (const std::string& pattern : patterns)
        {
            rx_patterns.emplace_back(pattern);
        }

        for (const auto& [name, sw] : this->get_switches())
        {
            for (const std::regex& rx : rx_patterns)
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

    SwitchMap Provider::find_glob_matches(
        const std::vector<std::string>& patterns) const
    {
        SwitchMap matches;
        for (const auto& [name, sw] : this->get_switches())
        {
            for (const std::string& pattern : patterns)
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

    std::shared_ptr<Provider> provider;
}  // namespace cc::platform::switchboard
