// -*- c++ -*-
//==============================================================================
/// @file package-index-vfs.c++
/// @brief Scan for local release packages
/// @author Tor Slettnes
//==============================================================================

#include "package-index-vfs.h++"
#include "upgrade-settings.h++"
#include "vfs.h++"
#include "status/exceptions.h++"

namespace upgrade::native
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

    PackageCatalogue VFSPackageIndex::scan()
    {
        logf_debug("Upgrade scan in VFS path: %r", this->vfs_path);
        vfs::Location loc = vfs::vfs->location(this->vfs_path, false);
        fs::path required_extension(
            this->settings->get(SETTING_PACKAGE_SUFFIX, DEFAULT_PACKAGE_SUFFIX).as_string());

        PackageCatalogue packages;

        for (const auto &pi : fs::directory_iterator(loc.localPath()))
        {
            fs::path filepath = pi.path();
            if (filepath.extension() == required_extension)
            {
                if (PackageInfo::ptr package_info = this->scan_file(loc, filepath.filename()))
                {
                    packages.push_back(package_info);
                    logf_debug("Adding upgrade package: %r", package_info->source());
                }
            }
        }
        this->available_packages = packages;
        return packages;
    }

    PackageInfo::ptr VFSPackageIndex::scan_file(
        const vfs::Location &location,
        const fs::path &package_file)
    {
        fs::path staging_folder = this->create_staging_folder();
        PackageInfo::ptr package_info;

        try
        {
            this->unpack_file(location.localPath(package_file), staging_folder);
            package_info = std::make_shared<NativePackageInfo>(
                staging_folder / this->package_info_file(),          // path
                PackageSource(location.virtualPath(package_file)));  // source
        }
        catch (...)
        {
            logf_info("Unable to scan VFS path %s: %s",
                      location.virtualPath(package_file),
                      std::current_exception());
        }

        fs::remove_all(staging_folder);
        return package_info;
    }

}  // namespace upgrade::native
