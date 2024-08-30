// -*- c++ -*-
//==============================================================================
/// @file package-handler.h++
/// @brief Generic upgrade package handler interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler.h++"
#include "upgrade-settings.h++"
#include "sysconfig-product.h++"
#include "sysconfig-host.h++"
#include "sysconfig-process.h++"
#include "status/exceptions.h++"
#include "io/streambuffer.h++"

#include <future>
#include <functional>
#include <fstream>

namespace platform::upgrade::native
{
    PackageHandler::PackageHandler(const core::SettingsStore::Ref &settings)
        : settings(settings)
    {
    }

    std::vector<PackageInfo::Ref> PackageHandler::get_available() const
    {
        return this->available_packages;
    }

    void PackageHandler::install(const PackageInfo::Ref &package_info)
    {
        fs::path staging_folder = this->create_staging_folder();
        std::exception_ptr eptr;

        try
        {
            this->unpack(package_info, staging_folder);
            this->install_unpacked(staging_folder);
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
        const fs::path &staging_folder)
    {
        PackageManifest manifest(staging_folder / this->manifest_file());
        this->check_applicable(manifest);
    }

    void PackageHandler::finalize(const PackageInfo::Ref &package_info)
    {
        if (package_info->reboot_required)
        {
            sysconfig::host->reboot();
        }
    }

    void PackageHandler::check_applicable(const PackageManifest &manifest) const
    {
        platform::sysconfig::ProductInfo pi = platform::sysconfig::product->get_product_info();
        if (!manifest.product_match(pi.product_name))
        {
            throw core::exception::FailedPrecondition(
                "Package does not match installed product");
        }
        else if (!manifest.version_match(pi.release_version))
        {
            throw core::exception::FailedPrecondition(
                "Package version is not newer than installed version");
        }
    }

    bool PackageHandler::is_applicable(
        const PackageManifest &manifest) const
    {
        try
        {
            this->check_applicable(manifest);
            return true;
        }
        catch (const core::exception::FailedPrecondition &)
        {
            return false;
        }
    }

    fs::path PackageHandler::create_staging_folder() const
    {
        return core::platform::path->mktemp();
    }

    fs::path PackageHandler::manifest_file() const
    {
        return this->settings->get(SETTING_MANIFEST_FILE, DEFAULT_MANIFEST_FILE).as_string();
    }

    PackageInfo::Ref PackageHandler::create_package_info(
        const PackageSource &package_source,
        const fs::path &package_name,
        const PackageManifest &manifest)
    {
        return std::make_shared<PackageInfo>(
            package_source,                  // source
            package_name,                    // package_name
            manifest.product_name(),         // product_name
            manifest.version(),              // release_version
            manifest.description(),          // description
            manifest.reboot_required(),      // reboot_required
            this->is_applicable(manifest));  // applicable
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
        core::platform::Invocations pipeline = {
            {
                .argv = {
                    "/usr/bin/gpgv",
                    "--output",
                    "-",
                    "--status-fd",
                    "2",
                },
            },
            {
                .argv = {
                    "/bin/tar",
                    "x",
                },
                .cwd = staging_folder,
            },
        };

        core::platform::process->pipe_from_stream(pipeline, stream);
    }

    core::platform::PID PackageHandler::invoke_decrypt(
        std::shared_ptr<std::istream> stream,
        int *stdin_fd,
        int *stdout_fd,
        int *stderr_fd)
    {
        core::platform::ArgVector argv = {
            "/usr/bin/gpgv",
            "--output",
            "-",
            "--status-fd",
            "2",
        };

        if (const core::types::Value &keyring = this->settings->get(SETTING_KEYRING_FILE))
        {
            argv.insert(
                argv.end(),
                {
                    "--keyring",
                    keyring.as_string(),
                });
        }

        core::platform::PID pid = core::platform::process->invoke_async_pipe(
            argv,
            {},
            stdin_fd,
            stdout_fd,
            stderr_fd);

        return pid;
    }

    core::platform::PID PackageHandler::invoke_unpack(
        int input_fd,
        const fs::path &staging_folder,
        int *stdout_fd,
        int *stderr_fd)
    {
        core::platform::ArgVector argv = {
            "/bin/tar",
            "x",
        };

        return core::platform::process->invoke_async_from_pipe(
            argv,
            staging_folder,
            input_fd,
            stdout_fd,
            stderr_fd);
    }

    void PackageHandler::write_from_stream(
        std::shared_ptr<std::istream> stream,
        int fd)
    {
        core::types::ByteVector buffer(core::io::BUFSIZE);
        while (stream->good())
        {
            stream->read(reinterpret_cast<char *>(buffer.data()), buffer.size());
            ::write(fd, buffer.data(), stream->gcount());
        }
        logf_debug("Wrote %d bytes from input stream to file descriptor %d",
                   stream->tellg(),
                   fd);
    }

}  // namespace platform::upgrade::native
