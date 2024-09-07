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

    PackageManifest::ptr PackageHandler::install(const PackageSource &source)
    {
        fs::path staging_folder = this->create_staging_folder();
        std::exception_ptr eptr;
        PackageManifest::ptr manifest;

        try
        {
            this->unpack(source.filename, staging_folder);
            manifest = this->install_unpacked(source, staging_folder);
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

        return manifest;
    }

    PackageManifest::ptr PackageHandler::install_unpacked(
        const PackageSource &source,
        const fs::path &staging_folder)
    {
        auto manifest = std::make_shared<LocalManifest>(
            staging_folder / this->manifest_file(),
            source);

        core::platform::ArgVector argv = manifest->install_command();
        logf_notice("Installing upgrade from %s: %s", staging_folder, argv);

        core::platform::FileDescriptor stdout_fd, stderr_fd;
        core::platform::PID pid = core::platform::process->invoke_async_pipe(
            argv,
            staging_folder,
            nullptr,
            &stdout_fd,
            &stderr_fd);

        std::future<void> stdout_future = std::async(
            &This::capture_pipe_output,
            this,
            stdout_fd,
            "stdout");

        std::future<void> stderr_future = std::async(
            &This::capture_pipe_output,
            this,
            stderr_fd,
            "stderr");

        core::platform::process->waitpid(pid, true);
        return manifest;
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

    void PackageHandler::capture_pipe_output(
        core::platform::FileDescriptor fd,
        const std::string output_name)
    {
        std::vector<char> buf(core::platform::CHUNKSIZE);

        while (std::size_t nbytes = core::platform::process->read(fd, buf.data(), buf.size()))
        {
            logf_info("Read %d bytes from %s: %s",
                      nbytes,
                      output_name,
                      std::string_view(buf.data(), nbytes));
        }
    }

}  // namespace platform::upgrade::native
