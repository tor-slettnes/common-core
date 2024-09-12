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
    VFSPackageHandler::VFSPackageHandler(const core::SettingsStore::ptr &settings,
                                         const vfs::Path &vfs_path)
        : Super(settings),
          vfs_path(vfs_path)
    {
    }

    PackageSource VFSPackageHandler::get_source() const
    {
        return {this->vfs_path};
    }

    PackageManifests VFSPackageHandler::scan()
    {
        logf_debug("Upgrade scan in VFS path: %r", this->vfs_path);
        vfs::Location loc = vfs::vfs->location(this->vfs_path, false);
        fs::path required_extension(
            this->settings->get(SETTING_PACKAGE_SUFFIX, DEFAULT_PACKAGE_SUFFIX).as_string());

        PackageManifests manifests;

        for (const auto &pi : fs::directory_iterator(loc.localPath()))
        {
            fs::path filepath = pi.path();
            if (filepath.extension() == required_extension)
            {
                if (const PackageManifest::ptr &manifest = this->scan_file(loc, filepath.filename()))
                {
                    manifests.push_back(manifest);
                    logf_debug("Adding upgrade package: %r", filepath);
                }
            }
        }
        this->available_packages = manifests;
        return manifests;
    }

    void VFSPackageHandler::unpack(const fs::path &filename,
                                   const fs::path &staging_folder)
    {
        if (filename.empty())
        {
            throw core::exception::MissingArgument("Missing package file");
        }

        vfs::Location loc = vfs::vfs->location(this->vfs_path, false);
        this->emit_upgrade_progress(UpgradeProgress::STATE_UNPACKING);
        this->unpack_file(loc.localPath(filename), staging_folder);
    }

    PackageManifest::ptr VFSPackageHandler::scan_file(const vfs::Location &location,
                                                      const fs::path &package_name)
    {
        fs::path staging_folder = this->create_staging_folder();
        PackageManifest::ptr manifest;

        try
        {
            this->unpack_file(location.localPath(package_name), staging_folder);
            manifest = std::make_shared<LocalManifest>(
                staging_folder / this->manifest_file(),              // manifest_file
                PackageSource(location.virtualPath(package_name)));  // source
        }
        catch (...)
        {
            logf_info("Unable to scan VFS path %s: %s",
                      location.virtualPath(package_name),
                      std::current_exception());
        }

        fs::remove_all(staging_folder);
        return manifest;
    }

}  // namespace platform::upgrade::native
