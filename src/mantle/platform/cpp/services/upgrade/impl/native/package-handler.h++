// -*- c++ -*-
//==============================================================================
/// @file package-handler.h++
/// @brief Generic upgrade package handler interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-manifest.h++"
#include "upgrade-types.h++"
#include "platform/path.h++"
#include "platform/process.h++"
#include "settings/settingsstore.h++"

namespace platform::upgrade::native
{
    class PackageHandler
    {
        using This = PackageHandler;

    public:
        using ptr = std::shared_ptr<This>;

    protected:
        PackageHandler(const core::SettingsStore::ptr &settings);

    public:
        virtual PackageManifests scan() = 0;
        virtual PackageSource get_source() const = 0;
        virtual std::vector<PackageManifest::ptr> get_available() const;
        virtual std::size_t get_available_count() const;
        virtual PackageManifest::ptr install(const PackageSource &source);
        virtual void finalize(const PackageManifest::ptr &package_info);

    protected:
        virtual void unpack(
            const fs::path &filename,
            const fs::path &staging_folder) = 0;

    protected:
        std::shared_ptr<LocalManifest> install_unpacked(
            const PackageSource &source,
            const fs::path &staging_folder);

        fs::path create_staging_folder() const;
        fs::path manifest_file() const;

        void unpack_file(
            const fs::path &filepath,
            const fs::path &staging_folder);

        void unpack_stream(
            std::istream &stream,
            const fs::path &staging_folder);

    private:
        void check_gpg_verify_result(
            const core::platform::Invocation &invocation,
            const core::platform::InvocationResult &result);

        void check_tar_unpack_result(
            const core::platform::Invocation &invocation,
            const core::platform::InvocationResult &result);

        void capture_install_progress(
            core::platform::FileDescriptor fd,
            std::shared_ptr<LocalManifest> manifest);

        void capture_install_diagnostics(
            core::platform::FileDescriptor fd,
            std::shared_ptr<LocalManifest> manifest);

    protected:
        void emit_scan_progress(
            const std::optional<PackageSource> &source) const;

        void emit_upgrade_progress(
            const std::optional<UpgradeProgress::State> &state = {},
            const std::optional<std::string> &task_description = {},
            const std::optional<UpgradeProgress::Fraction> &task_progress = {},
            const std::optional<UpgradeProgress::Fraction> &total_progress = {},
            const std::optional<core::status::Event::ptr> error = {}) const;

    protected:
        std::vector<PackageManifest::ptr> available_packages;
        core::SettingsStore::ptr settings;
        std::stringstream install_diagnostics;
    };
}  // namespace platform::upgrade::native
