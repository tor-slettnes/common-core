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

    constexpr auto SETTING_PACKAGE_SUFFIX = "package suffix";
    constexpr auto DEFAULT_PACKAGE_SUFFIX = ".pkg";

    constexpr auto SETTING_MANIFEST_FILE = "manifest file";
    constexpr auto DEFAULT_MANIFEST_FILE = "manifest.json";

    constexpr auto SETTING_VFS_CONTEXT = "vfs context";
    constexpr auto DEFAULT_VFS_CONTEXT = "RELEASES";

    constexpr auto SETTING_DOWNLOAD_URL = "download url";
    constexpr auto SETTING_VERIFY_COMMAND = "verify command";
    constexpr auto SETTING_UNPACK_COMMAND = "unpack command";
    constexpr auto SETTING_KEYRING_FILE = "keyring file";

}
