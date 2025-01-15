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

namespace core
{
    extern std::shared_ptr<core::SettingsStore> settings;

    void init_settings(const std::optional<std::string> &flavor = {});
}  // namespace core
