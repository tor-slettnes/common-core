/// -*- c++ -*-
//==============================================================================
/// @file settings.h++
/// @brief Global settings instance
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "settingsstore.h++"
#include "platform/path.h++"

#include <memory>

namespace cc
{
    extern std::shared_ptr<cc::SettingsStore> settings;

    void init_settings(const types::PathList &files = {platform::path->exec_name(true), "defaults"});
}  // namespace cc
