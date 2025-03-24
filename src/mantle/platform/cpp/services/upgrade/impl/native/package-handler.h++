// -*- c++ -*-
//==============================================================================
/// @file package-handler.h++
/// @brief Generic upgrade package handler interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-info.h++"
#include "upgrade-types.h++"
#include "platform/path.h++"
#include "platform/process.h++"
#include "settings/settingsstore.h++"

namespace upgrade::native
{
    class PackageHandler
    {
        using This = PackageHandler;

    public:
        using ptr = std::shared_ptr<This>;

    protected:
        PackageHandler(const core::SettingsStore::ptr &settings);

    public:
        virtual PackageInfo::ptr install(const PackageSource &source);
        virtual void finalize(const PackageInfo::ptr &package_info);

    protected:
        virtual void unpack(
            const PackageSource &source,
            const fs::path &staging_folder) = 0;

    protected:
        std::shared_ptr<NativePackageInfo> install_unpacked(
            const PackageSource &source,
            const fs::path &staging_folder);

        fs::path create_staging_folder() const;
        fs::path package_info_file() const;

        void unpack_from_fd(
            core::platform::FileDescriptor fd,
            const fs::path &staging_folder) const;

    private:
        void check_gpg_verify_result(
            const core::platform::Invocation &invocation,
            const core::platform::InvocationResult &result) const;

        void check_tar_unpack_result(
            const core::platform::Invocation &invocation,
            const core::platform::InvocationResult &result) const;

        void capture_install_progress(
            core::platform::FileDescriptor fd,
            std::shared_ptr<NativePackageInfo> package_info) const;

        void capture_install_diagnostics(
            core::platform::FileDescriptor fd,
            std::shared_ptr<NativePackageInfo> package_info);

    protected:
        void emit_upgrade_progress(
            const std::optional<UpgradeProgress::State> &state = {},
            const std::optional<std::string> &task_description = {},
            const std::optional<UpgradeProgress::Fraction> &task_progress = {},
            const std::optional<UpgradeProgress::Fraction> &total_progress = {},
            const std::optional<core::status::Error::ptr> error = {}) const;

    protected:
        core::SettingsStore::ptr settings;
        std::shared_ptr<std::stringstream> install_diagnostics;
    };
}  // namespace upgrade::native
