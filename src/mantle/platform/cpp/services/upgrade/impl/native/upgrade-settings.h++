// -*- c++ -*-
//==============================================================================
/// @file upgrade-settings.h++
/// @brief Upgrade service settings
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "settings/settingsstore.h++"

namespace platform::upgrade::native
{
    constexpr auto SETTINGS_FILE = "upgrade.json";

    constexpr auto SETTING_KEYRING_FILE = "keyring file";

    constexpr auto SETTING_PACKAGE_SUFFIX = "package suffix";
    constexpr auto DEFAULT_PACKAGE_SUFFIX = ".pkg";

    constexpr auto SETTING_MANIFEST_FILE = "manifest file";
    constexpr auto DEFAULT_MANIFEST_FILE = "manifest.json";

    constexpr auto SETTING_VFS_CONTEXT = "vfs context";
    constexpr auto DEFAULT_VFS_CONTEXT = "releases";

    constexpr auto SETTING_SCAN_URL = "scan url";

    constexpr auto SETTING_SCAN_INTERVAL = "scan interval";
    constexpr auto DEFAULT_SCAN_INTERVAL = 86400;

    constexpr auto SETTING_SCAN_RETRIES = "scan retries";
    constexpr auto DEFAULT_SCAN_RETRIES = 10;

}  // namespace platform::upgrade::native
