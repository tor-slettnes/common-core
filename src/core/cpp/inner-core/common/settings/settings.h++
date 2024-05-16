/// -*- c++ -*-
//==============================================================================
/// @file settings.h++
/// @brief Global settings instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "settingsstore.h++"
#include "platform/init.h++"

#include <memory>

namespace cc
{
    extern std::shared_ptr<cc::SettingsStore> settings;

    void init_settings();
}  // namespace cc
