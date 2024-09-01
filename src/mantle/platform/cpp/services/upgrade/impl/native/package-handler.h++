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
#include "settings/settingsstore.h++"

namespace platform::upgrade::native
{
    class PackageHandler
    {
        using This = PackageHandler;

    protected:
        PackageHandler(const core::SettingsStore::Ref &settings);

    public:
        virtual void scan() = 0;
        virtual std::vector<PackageInfo::Ref> get_available() const;
        virtual void install(const PackageSource &source);
        virtual void finalize(const PackageInfo::Ref &package_info);

    protected:
        virtual void unpack(
            const PackageSource &source,
            const fs::path &staging_folder) = 0;

    protected:
        void install_unpacked(
            const fs::path &staging_folder);

        void check_applicable(
            const PackageManifest &manifest) const;

        bool is_applicable(
            const PackageManifest &manifest) const;

        fs::path create_staging_folder() const;
        fs::path manifest_file() const;

        PackageInfo::Ref create_package_info(
            const PackageSource &source,
            const PackageManifest &manifest);

        void unpack_file(
            const fs::path &filepath,
            const fs::path &staging_folder);

        void unpack_stream(
            std::istream &stream,
            const fs::path &staging_folder);

    private:
        core::platform::PID invoke_decrypt(
            std::shared_ptr<std::istream> stream,
            int *stdin_fd,
            int *stdout_fd,
            int *stderr_fd);

        core::platform::PID invoke_unpack(
            int input_fd,
            const fs::path &staging_folder,
            int *stdout_fd,
            int *stderr_fd);

        void write_from_stream(
            std::shared_ptr<std::istream> stream,
            int fd);

    protected:
        std::vector<PackageInfo::Ref> available_packages;
        core::SettingsStore::Ref settings;
    };
}  // namespace platform::upgrade::native
