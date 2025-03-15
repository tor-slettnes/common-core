// -*- c++ -*-
//==============================================================================
/// @file switchboard-central.c++
/// @brief Switchboard provider supporting only local switches
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "switchboard-central.h++"
#include "local-switch.h++"
#include "switch-dependency.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"

namespace switchboard
{
    constexpr auto SETTINGS_MAIN_FILE = "switchboard";
    constexpr auto SETTING_SWITCH_CONFIG_FILES = "switch config files";

    Central::Central()
        : Super(TYPE_NAME_FULL(This)),
          settings_(SETTINGS_MAIN_FILE)
    {
    }

    void Central::initialize()
    {
        Super::initialize();
        logf_debug("Loading switches from: %s", this->settings_);
        if (const auto &configfiles = this->settings_.get(SETTING_SWITCH_CONFIG_FILES).get_valuelist())
        {
            for (const auto &filename : *configfiles)
            {
                try
                {
                    this->load(filename.as_string());
                }
                catch (const std::exception &e)
                {
                    logf_warning("Failed to load switches from %r: %s", filename, e);
                }
            }
        }
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
        const SwitchName &switch_name)
    {
        return this->find_or_insert<LocalSwitch>(
            switch_name,
            this->shared_from_this());
    }

    bool Central::remove_switch(
        const SwitchName &switch_name,
        bool propagate)
    {
        bool erased = this->switches.erase(switch_name);
        if (erased)
        {
            logf_info("Removed switch: %r", switch_name);
            for (const auto &[candidate, sw] : this->switches)
            {
                sw->remove_dependency(switch_name, propagate);
            }
        }
        return erased;
    }

}  // namespace switchboard
