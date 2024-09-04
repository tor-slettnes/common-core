// -*- c++ -*-
//==============================================================================
/// @file package-handler-vfs.h++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-handler.h++"
#include "vfs-context.h++"
#include "types/create-shared.h++"

namespace platform::upgrade::native
{
    class VFSPackageHandler : public PackageHandler,
                              public core::types::enable_create_shared<VFSPackageHandler>
    {
        using This = VFSPackageHandler;
        using Super = PackageHandler;

    public:
        VFSPackageHandler(const core::SettingsStore::ptr &settings,
                          const vfs::Path &vfs_path);

    public:
        PackageSource get_source() const override;
        void scan() override;
        void unpack(const PackageSource &source,
                    const fs::path &staging_folder) override;

    private:
        PackageManifest::ptr scan_file(const vfs::Location &location,
                                       const fs::path &package_name);

    private:
        vfs::Path vfs_path;
    };
}  // namespace platform::upgrade::native
