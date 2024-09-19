// -*- c++ -*-
//==============================================================================
/// @file package-index-vfs.h++
/// @brief Scan for local release packages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-index.h++"
#include "package-handler-vfs.h++"
#include "settings/settingsstore.h++"

namespace platform::upgrade::native
{
    class VFSPackageIndex : public PackageIndex,
                            public VFSPackageHandler
    {
        using This = VFSPackageIndex;
        using Super = PackageIndex;

    public:
        VFSPackageIndex(const core::SettingsStore::ptr &settings,
                        const vfs::Path &vfs_path);

    public:
        PackageSource package_source(const std::string &package_name) const override;
        PackageManifests scan() override;

    private:
        PackageManifest::ptr scan_file(
            const vfs::Location &location,
            const fs::path &package_name);

    private:
        vfs::Path vfs_path;
    };
}  // namespace platform::upgrade::native
