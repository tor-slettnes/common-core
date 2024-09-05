// -*- c++ -*-
//==============================================================================
/// @file package-manifest.h++
/// @brief Manifest describing package contents
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "platform/process.h++"
#include "settings/settingsstore.h++"
#include "types/create-shared.h++"

namespace platform::upgrade::native
{
    //==========================================================================
    // Package Manifest

    class LocalManifest : public PackageManifest
    {
        using This = LocalManifest;
        using Super = PackageManifest;

    public:
        LocalManifest(const core::SettingsStore &settings,
                      const PackageSource &source);

    public:
        const bool is_applicable() const override;
        void check_applicable() const;

        core::platform::ArgVector install_command() const;

    private:
        static Version decode_version(const core::types::Value &value);
        static std::string decode_description(const core::types::Value &value);

        bool is_applicable_product(const std::string &current_product) const;
        bool is_applicable_version(const Version &current_version) const;

    protected:
        std::string product_match;
        std::string version_match;
        core::types::Value command;
    };

}  // namespace platform::upgrade::native
