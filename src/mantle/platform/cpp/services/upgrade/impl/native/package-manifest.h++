// -*- c++ -*-
//==============================================================================
/// @file package-manifest.h++
/// @brief Manifest describing package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "settings/settingsstore.h++"

namespace platform::upgrade::native
{
    constexpr auto SETTING_PRODUCT = "product";
    constexpr auto SETTING_PRODUCT_MATCH = "product match";
    constexpr auto SETTING_VERSION = "version";
    constexpr auto SETTING_VERSION_MATCH = "version match";
    constexpr auto SETTING_DESCRIPTION = "description";
    constexpr auto SETTING_REBOOT = "reboot required";

    //==========================================================================
    // Package Manifest

    class PackageManifest : public core::SettingsStore
    {
        using This = PackageManifest;
        using Super = core::SettingsStore;

    protected:
        using Super::Super;

    public:
        std::string product_name(const std::string &fallback={}) const;
        Version version(const Version &fallback={}) const;
        std::string description(const std::string &fallback={}) const;
        bool reboot_required(bool fallback=false) const;

        bool product_match(const std::string &installed_product) const;
        bool version_match(const Version &installed_version) const;
    };

}  // namespace platform::upgrade::native
