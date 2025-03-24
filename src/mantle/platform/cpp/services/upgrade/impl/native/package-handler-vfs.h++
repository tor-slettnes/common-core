// -*- c++ -*-
//==============================================================================
/// @file package-handler-vfs.h++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-handler.h++"
#include "vfs-location.h++"

namespace upgrade::native
{
    class VFSPackageHandler : public PackageHandler
    {
        using This = VFSPackageHandler;
        using Super = PackageHandler;

    public:
        VFSPackageHandler(const core::SettingsStore::ptr &settings);

    public:
        void unpack(
            const PackageSource &source,
            const fs::path &staging_folder) override;

    protected:
        void unpack_file(
            const fs::path &filepath,
            const fs::path &staging_folder);
    };
}  // namespace upgrade::native
