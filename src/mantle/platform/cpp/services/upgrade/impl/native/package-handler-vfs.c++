// -*- c++ -*-
//==============================================================================
/// @file package-handler-vfs.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler-vfs.h++"
#include "upgrade-settings.h++"
#include "vfs.h++"

namespace platform::upgrade::native
{
    VFSPackageHandler::VFSPackageHandler(const core::SettingsStore::Ref &settings,
                                         const vfs::Path &vfs_path)
        : Super(settings),
          vfs_path(vfs_path)
    {
    }

    void VFSPackageHandler::scan()
    {
        vfs::Location loc = vfs::location(this->vfs_path, false);
        fs::path required_extension(
            this->settings->get(SETTING_PACKAGE_SUFFIX, DEFAULT_PACKAGE_SUFFIX).as_string());

        // for (const auto &[name, stat]: vfs::get_directory(this->vfs_path, false))
        // {
        //     if (name.extension() == required_extension)
        //     {
        //         this->scan_file(name);
        //     }
        // }
        for (const auto &pi : fs::directory_iterator(loc.localPath()))
        {
            fs::path filepath = pi.path();
            if (filepath.extension() == required_extension)
            {
                this->scan_file(loc, filepath.filename());
            }
        }
    }


    void VFSPackageHandler::unpack(const PackageInfo::Ref &info,
                                   const fs::path &staging_folder)
    {
        vfs::Location loc = vfs::location(std::get<vfs::Path>(info->source), false);
        unpack_file(loc.localPath(info->package_name), staging_folder);
    }

    PackageInfo::Ref VFSPackageHandler::scan_file(const vfs::Location &location,
                                                  const fs::path &package_name)
    {
        fs::path staging_folder = this->create_staging_folder();

        try
        {
            this->unpack_file(location.localPath(package_name), staging_folder);
            PackageManifest manifest(staging_folder / this->manifest_file());
            fs::remove_all(staging_folder);
            return this->create_package_info(location.virtualPath(),
                                             package_name,
                                             manifest);
        }
        catch (...)
        {
            fs::remove_all(staging_folder);
            throw;
        }
    }

}  // namespace platform::upgrade::native
