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


    void VFSPackageHandler::scan()
    {
        logf_info("Upgrade scan in VFS path: %r", this->vfs_path);
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
                logf_debug("Scanning upgrade package: %r", filepath);
                this->scan_file(loc, filepath.filename());
                logf_debug("Scanned upgrade package: %r", filepath);
            }
        }
    }

    void VFSPackageHandler::unpack(const PackageSource &source,
                                   const fs::path &staging_folder)
    {
        vfs::Location loc = vfs::location(source.vfs_path().value(), false);
        unpack_file(loc.localPath(source.filename), staging_folder);
    }

    PackageManifest::ptr VFSPackageHandler::scan_file(const vfs::Location &location,
                                                      const fs::path &package_name)
    {
        fs::path staging_folder = this->create_staging_folder();

        try
        {
            this->unpack_file(location.localPath(package_name), staging_folder);
            auto manifest = std::make_shared<LocalManifest>(
                staging_folder / this->manifest_file(),                // manifest_file
                PackageSource(location.virtualPath(), package_name));  // source

            // fs::remove_all(staging_folder);
            return manifest;
        }
        catch (...)
        {
            // fs::remove_all(staging_folder);
            throw;
        }
    }

}  // namespace platform::upgrade::native
