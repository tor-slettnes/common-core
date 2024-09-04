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

    public:
        using Ptr = std::shared_ptr<This>;

    protected:
        PackageHandler(const core::SettingsStore::ptr &settings);

    public:
        virtual void scan() = 0;
        virtual PackageSource get_source() const = 0;
        virtual std::vector<PackageManifest::ptr> get_available() const;
        virtual std::size_t get_available_count() const;
        virtual void install(const PackageSource &source);
        virtual void finalize(const PackageManifest::ptr &package_info);

    protected:
        virtual void unpack(
            const PackageSource &source,
            const fs::path &staging_folder) = 0;

    protected:
        void install_unpacked(
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

    protected:
        std::vector<PackageManifest::ptr> available_packages;
        core::SettingsStore::ptr settings;
    };
}  // namespace platform::upgrade::native
