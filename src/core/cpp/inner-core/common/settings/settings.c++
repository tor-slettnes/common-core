/// -*- c++ -*-
//==============================================================================
/// @file settings.c++
/// @brief Global settings instance
/// @author Tor Slettnes
//==============================================================================

#include "settings.h++"
#include "status/exceptions.h++"
#include "platform/path.h++"
#include "platform/init.h++"

#include <iostream>

namespace core
{
    std::shared_ptr<core::SettingsStore> settings;

    void init_settings(const std::string &primary_settings_file,
                       const std::optional<std::string> &flavor)
    {
        if (!settings)
        {
            settings = SettingsStore::create_shared();
            try
            {
                settings->load(primary_settings_file);
                if (flavor)
                {
                    settings->load(flavor.value() + "-defaults");
                }
                settings->load("defaults");
            }
            catch (const std::exception &e)
            {
                std::cerr << e << std::endl;
            }
        }
    }

    // static platform::InitTask init_app_settings("init_settings", init_settings);

}  // namespace core
