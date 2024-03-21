/// -*- c++ -*-
//==============================================================================
/// @file settings.c++
/// @brief Global settings instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "settings.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"
#include "platform/init.h++"

#include <iostream>

namespace core
{
    std::shared_ptr<core::SettingsStore> settings;

    void init_settings()
    {
        types::PathList files = {platform::path->exec_name(true), "defaults"};

        if (!settings)
        {
            settings = SettingsStore::create_shared();
            try
            {
                settings->load(files);
            }
            catch (const std::exception &e)
            {
                std::cerr << e << std::endl;
            }
        }
    }

    static platform::InitTask init_app_settings("init_settings", init_settings);

}  // namespace core
