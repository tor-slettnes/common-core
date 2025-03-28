// -*- c++ -*-
//==============================================================================
/// @file upgrade-settings.c++
/// @brief Upgrade service settings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-settings.h++"
#include "platform/init.h++"
#include "status/exceptions.h++"

namespace upgrade::native
{
    core::SettingsStore::ptr settings;

    void init_settings()
    {
        settings = core::SettingsStore::create_shared();
        try
        {
            settings->load(SETTINGS_FILE);
        }
        catch (const std::exception &e)
        {
            std::cerr << e << std::endl;
        }
    }

}  // namespace upgrade::native
