// -*- c++ -*-
//==============================================================================
/// @file package-handler-vfs.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler-vfs.h++"
#include "upgrade-settings.h++"
#include "vfs.h++"
#include "status/exceptions.h++"

namespace platform::upgrade::native
{
    VFSPackageHandler::VFSPackageHandler(
        const core::SettingsStore::ptr &settings)
        : Super(settings)
    {
    }

    void VFSPackageHandler::unpack(
        const PackageSource &source,
        const fs::path &staging_folder)
    {
        if (source.empty())
        {
            throw core::exception::MissingArgument("Missing package file");
        }

        vfs::Location loc = vfs::vfs->location(source.vfs_path(), false);
        this->emit_upgrade_progress(UpgradeProgress::STATE_UNPACKING);
        this->unpack_file(loc.localPath(), staging_folder);
    }

    void VFSPackageHandler::unpack_file(
        const fs::path &filepath,
        const fs::path &staging_folder)
    {
        logf_debug("Unpacking file %r", filepath);
        this->unpack_from_fd(
            core::platform::process->open_read(filepath),
            staging_folder);
    }

}  // namespace platform::upgrade::native
