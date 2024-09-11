// -*- c++ -*-
//==============================================================================
/// @file package-handler.h++
/// @brief Generic upgrade package handler interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler.h++"
#include "upgrade-settings.h++"
#include "upgrade-signals.h++"
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
        signal_upgrade_progress.clear_cached();

        logf_notice("Installing upgrade: %s (filename=%s)",
                    source,
                    source.filename());

        try
        {
            this->unpack(source.filename(), staging_folder);
            manifest = this->install_unpacked(source, staging_folder);
        }
        catch (...)
        {
            eptr = std::current_exception();
        }

        fs::remove_all(staging_folder);

        if (eptr)
        {
            logf_error("Upgrade from %s failed: %s", source, eptr);
            std::rethrow_exception(eptr);
        }

        logf_notice("Upgrade succeeded: %s", *manifest);
        return manifest;
    }

    std::shared_ptr<LocalManifest> PackageHandler::install_unpacked(
        const PackageSource &source,
        const fs::path &staging_folder)
    {
        auto manifest = std::make_shared<LocalManifest>(
            staging_folder / this->manifest_file(),
            source);

        core::platform::ArgVector argv = manifest->install_command();

        signal_upgrade_pending.emit(manifest);
        this->emit_upgrade_progress(UpgradeProgress::STATE_INSTALLING);

        core::platform::FileDescriptor stdout_fd, stderr_fd;
        core::platform::PID pid = core::platform::process->invoke_async_pipe(
            argv,
            staging_folder,
            nullptr,
            &stdout_fd,
            &stderr_fd);

        std::future<void> stdout_future = std::async(
            &This::capture_install_progress,
            this,
            stdout_fd,
            manifest);

        std::future<void> stderr_future = std::async(
            &This::capture_install_diagnostics,
            this,
            stderr_fd,
            manifest);

        core::platform::ExitStatus::ptr err = core::platform::process->waitpid(pid);

        // Clear any pending upgrade information.
        signal_upgrade_pending.emit({});

        if (err->success())
        {
            this->emit_upgrade_progress(UpgradeProgress::STATE_COMPLETED);
            return manifest;
        }
        else
        {
            auto error = std::make_shared<core::exception::InvocationError>(
                argv.front(),
                err->combined_code(),
                "",
                core::str::strip(this->install_diagnostics.str()));

            this->emit_upgrade_progress(
                UpgradeProgress::STATE_FAILED,  // state
                {},                             // task_description
                {},                             // task_progress
                {},                             // total_progress
                error);                         // error

            throw *error;
        }
    }

    void PackageHandler::finalize(const PackageManifest::ptr &package_info)
    {
        if (package_info->reboot_required())
        {
            sysconfig::host->reboot();
        }
        this->emit_upgrade_progress(UpgradeProgress::STATE_FINALIZED);
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
        logf_debug("Unpacking file %r", filepath);
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

            if (!fs::exists(keyring.as_string()))
            {
                throwf(core::exception::FailedPrecondition,
                       "Unable to verify package signature: Keyring %r not found",
                       keyring);
            }
        }

        core::platform::ArgVector argv_tar = {
            "/bin/tar",
            "x",
        };

        core::platform::Invocations pipeline = {
            {argv_gpgv, {}},
            {argv_tar, staging_folder},
        };

        core::platform::InvocationResults results =
            core::platform::process->pipe_from_stream(pipeline, stream);

        this->check_gpg_verify_result(pipeline.at(0), results.at(0));
        this->check_tar_unpack_result(pipeline.at(1), results.at(1));
    }

    void PackageHandler::check_gpg_verify_result(
        const core::platform::Invocation &invocation,
        const core::platform::InvocationResult &result)
    {
        switch (result.error_code())
        {
        case 0:
            logf_debug("Good package signature");
            break;

        case 2:
            throw core::exception::FailedPrecondition("Invalid package signature");

        default:
            throw core::exception::InvocationError("Unable to verify package signature",
                                                   result);
        }
    }

    void PackageHandler::check_tar_unpack_result(
        const core::platform::Invocation &invocation,
        const core::platform::InvocationResult &result)
    {
        if (result.error_code())
        {
            throw core::exception::InvocationError("Unable to extract package contents",
                                                   result);
        }
    }


    void PackageHandler::capture_install_progress(
        core::platform::FileDescriptor fd,
        std::shared_ptr<LocalManifest> manifest)
    {
        std::vector<char> buf(core::platform::CHUNKSIZE);
        std::regex rx_total_progress(manifest->match_capture_total_progress());
        std::regex rx_task_progress(manifest->match_capture_task_progress());
        std::regex rx_task_description(manifest->match_capture_task_description());
        std::smatch match;

        UpgradeProgress::State state = UpgradeProgress::STATE_INSTALLING;

        while (std::size_t nbytes = core::platform::process->readfd(
                   fd,
                   buf.data(),
                   buf.size()))
        {
            std::string text(buf.data(), nbytes);
            logf_trace("Read %d bytes from stdout: %s", nbytes, core::str::strip(text));

            if (std::regex_search(text, match, rx_total_progress))
            {
                UpgradeProgress::Fraction progress(
                    core::types::Value::from_literal(match.str(1)),
                    core::types::Value::from_literal(match.str(2)));

                this->emit_upgrade_progress(
                    state,     // state
                    {},        // task_description
                    {},        // task_progress
                    progress,  // total_progress
                    {});       // error
            }

            else if (std::regex_search(text, match, rx_task_progress))
            {
                UpgradeProgress::Fraction progress(
                    core::types::Value::from_literal(match.str(1)),
                    core::types::Value::from_literal(match.str(2)));

                this->emit_upgrade_progress(
                    state,     // state
                    {},        // task_description
                    progress,  // task_progress
                    {},        // total_progress
                    {});       // error
            }

            else if (std::regex_search(text, match, rx_task_description))
            {
                this->emit_upgrade_progress(
                    state,         // state
                    match.str(1),  // task_description
                    {},            // task_progress
                    {},            // total_progress
                    {});           // error
            }
        }
    }

    void PackageHandler::capture_install_diagnostics(
        core::platform::FileDescriptor fd,
        std::shared_ptr<LocalManifest> manifest)
    {
        this->install_diagnostics = {};
        std::vector<char> buf(core::platform::CHUNKSIZE);

        while (std::size_t nbytes = core::platform::process->readfd(
                   fd,
                   buf.data(),
                   buf.size()))
        {
            logf_info("Read %d bytes from stderr: %s",
                      nbytes,
                      std::string_view(buf.data(), nbytes));
            this->install_diagnostics.write(buf.data(), nbytes);
        }
    }

    void PackageHandler::emit_scan_progress(
        const std::optional<PackageSource> &source) const
    {
        auto progress = std::make_shared<ScanProgress>();
        if (source)
        {
            progress->source = source.value();
        }
        signal_scan_progress.emit(progress);
    }

    void PackageHandler::emit_upgrade_progress(
        const std::optional<UpgradeProgress::State> &state,
        const std::optional<std::string> &task_description,
        const std::optional<UpgradeProgress::Fraction> &task_progress,
        const std::optional<UpgradeProgress::Fraction> &total_progress,
        const std::optional<core::status::Event::ptr> error) const
    {
        UpgradeProgress::ptr progress = signal_upgrade_progress.get_cached({});

        if (!progress)
        {
            progress = std::make_shared<UpgradeProgress>();
        }

        if (state)
        {
            progress->state = state.value();
        }

        if (task_description)
        {
            progress->task_description = task_description.value();
        }

        if (task_progress)
        {
            progress->task_progress = task_progress.value();
        }

        if (total_progress)
        {
            progress->total_progress = total_progress.value();
        }

        if (error)
        {
            progress->error = error.value();
        }

        signal_upgrade_progress.emit(progress);
    }

}  // namespace platform::upgrade::native
