// -*- c++ -*-
//==============================================================================
/// @file package-manifest.h++
/// @brief Manifest describing package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "settings/settingsstore.h++"
#include "types/create-shared.h++"

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

    class LocalManifest : public core::SettingsStore,
                          public PackageManifest
    {
        using This = LocalManifest;
        using Super = PackageManifest;

    public:
        LocalManifest(const fs::path &manifest_file,
                      const PackageSource &source);

    public:
        const bool is_applicable() const override;
        void check_applicable() const;

    private:
        static Version decode_version(const core::types::Value &value);
        static std::string decode_description(const core::types::Value &value);

        bool product_match(const std::string &current_product) const;
        bool version_match(const Version &current_version) const;
    };

}  // namespace platform::upgrade::native
