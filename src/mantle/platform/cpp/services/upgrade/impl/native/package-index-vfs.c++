// -*- c++ -*-
//==============================================================================
/// @file package-index-vfs.c++
/// @brief Scan for local release packages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-index-vfs.h++"
#include "upgrade-settings.h++"
#include "vfs.h++"
#include "status/exceptions.h++"

namespace platform::upgrade::native
{
    VFSPackageIndex::VFSPackageIndex(
        const core::SettingsStore::ptr &settings,
        const vfs::Path &vfs_path)
        : VFSPackageHandler(settings),
          vfs_path(vfs_path)
    {
    }

    PackageSource VFSPackageIndex::package_source(const std::string &package_name) const
    {
        return {this->vfs_path / package_name};
    }

    PackageManifests VFSPackageIndex::scan()
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
                if (PackageManifest::ptr manifest = this->scan_file(loc, filepath.filename()))
                {
                    manifests.push_back(manifest);
                    logf_debug("Adding upgrade package: %r", manifest->source());
                }
            }
        }
        this->available_packages = manifests;
        return manifests;
    }

    PackageManifest::ptr VFSPackageIndex::scan_file(
        const vfs::Location &location,
        const fs::path &package_file)
    {
        fs::path staging_folder = this->create_staging_folder();
        PackageManifest::ptr manifest;

        try
        {
            this->unpack_file(location.localPath(package_file), staging_folder);
            manifest = std::make_shared<LocalManifest>(
                staging_folder / this->manifest_file(),              // manifest_file
                PackageSource(location.virtualPath(package_file)));  // source
        }
        catch (...)
        {
            logf_info("Unable to scan VFS path %s: %s",
                      location.virtualPath(package_file),
                      std::current_exception());
        }

        fs::remove_all(staging_folder);
        return manifest;
    }

}  // namespace platform::upgrade::native
