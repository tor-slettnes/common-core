// -*- c++ -*-
//==============================================================================
/// @file package-handler.h++
/// @brief Generic upgrade package handler interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler.h++"
#include "upgrade-settings.h++"
#include "sysconfig-host.h++"
#include "sysconfig-process.h++"
#include "status/exceptions.h++"
#include "io/streambuffer.h++"

#include <future>
#include <functional>
#include <fstream>

namespace platform::upgrade::native
{
    PackageHandler::PackageHandler(const core::SettingsStore::ptr &settings)
        : settings(settings)
    {
    }

    std::vector<PackageManifest::ptr> PackageHandler::get_available() const
    {
        return this->available_packages;
    }

    std::size_t PackageHandler::get_available_count() const
    {
        return this->available_packages.size();
    }

    void PackageHandler::install(const PackageSource &source)
    {
        fs::path staging_folder = this->create_staging_folder();
        std::exception_ptr eptr;

        try
        {
            this->unpack(source.filename, staging_folder);
            this->install_unpacked(source, staging_folder);
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        fs::remove_all(staging_folder);

        if (eptr)
        {
            std::rethrow_exception(eptr);
        }
    }

    void PackageHandler::install_unpacked(
        const PackageSource &source,
        const fs::path &staging_folder)
    {
        LocalManifest manifest(staging_folder / this->manifest_file(), source);
        core::platform::ArgVector argv = manifest.install_command();
    }

    void PackageHandler::finalize(const PackageManifest::ptr &package_info)
    {
        if (package_info->reboot_required())
        {
            sysconfig::host->reboot();
        }
    }

    fs::path PackageHandler::create_staging_folder() const
    {
        return core::platform::path->mktempdir("upgrade.");
    }

    fs::path PackageHandler::manifest_file() const
    {
        return this->settings->get(SETTING_MANIFEST_FILE, DEFAULT_MANIFEST_FILE).as_string();
    }

    void PackageHandler::unpack_file(
        const fs::path &filepath,
        const fs::path &staging_folder)
    {
        std::ifstream stream(filepath, std::ios_base::binary);
        this->unpack_stream(stream, staging_folder);
    }

    void PackageHandler::unpack_stream(
        std::istream &stream,
        const fs::path &staging_folder)
    {
        core::platform::ArgVector argv_gpgv = {
            "/usr/bin/gpgv",
            "--output",
            "-",
            "--status-fd",
            "2",
        };

        if (const core::types::Value &keyring = this->settings->get(SETTING_KEYRING_FILE))
        {
            argv_gpgv.insert(
                argv_gpgv.end(),
                {
                    "--keyring",
                    keyring.as_string(),
                });
        }

        core::platform::ArgVector argv_tar = {
            "/bin/tar",
            "x",
        };

        core::platform::Invocations pipeline = {
            {argv_gpgv, {}},
            {argv_tar, staging_folder},
        };

        core::platform::process->pipe_from_stream(pipeline, stream);
    }

}  // namespace platform::upgrade::native
