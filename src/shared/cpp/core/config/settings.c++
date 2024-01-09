/// -*- c++ -*-
//==============================================================================
/// @file settings.c++
/// @brief Global settings instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "settings.h++"
#include "status/exceptions.h++"

#include <iostream>

namespace cc
{
    std::shared_ptr<cc::SettingsStore> settings;

    void init_settings(const types::PathList &files)
    {
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
}  // namespace cc
