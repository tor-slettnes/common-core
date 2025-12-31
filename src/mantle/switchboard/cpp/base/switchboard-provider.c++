// -*- c++ -*-
//==============================================================================
/// @file switchboard-provider.c++
/// @brief Abstract switchboard provider API
/// @author Tor Slettnes
//==============================================================================

#include "switchboard-provider.h++"
#include "settings/settingsstore.h++"
#include "status/exceptions.h++"

namespace switchboard
{
    constexpr auto SETTINGS_SECTION_SWITCHES = "switches";

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
            uint count = this->import_switches(*switches);
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


    std::shared_ptr<Provider> provider;
}  // namespace switchboard
