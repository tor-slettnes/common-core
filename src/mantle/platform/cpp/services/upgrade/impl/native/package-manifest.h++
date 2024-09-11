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
        LocalManifest(const fs::path &path,
                      const PackageSource &source = {});

        LocalManifest(const core::SettingsStore::ptr &settings,
                      const PackageSource &source = {});

    public:
        bool is_applicable() const override;
        void check_applicable() const;

        core::platform::ArgVector install_command() const;
        std::string match_capture_total_progress() const;
        std::string match_capture_task_progress() const;
        std::string match_capture_task_description() const;

    private:
        static Version decode_version(const core::types::Value &value);
        static std::string decode_description(const core::types::Value &value);

        std::string capture_setting(const std::string &setting,
                                    const std::string &fallback={}) const;

        bool is_applicable_product(const std::string &current_product) const;
        bool is_applicable_version(const Version &current_version) const;

    protected:
        core::SettingsStore::ptr settings;
    };

}  // namespace platform::upgrade::native
