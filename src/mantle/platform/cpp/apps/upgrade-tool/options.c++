// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Upgrade service tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "upgrade-provider-native.h++"
#include "upgrade-provider-grpc.h++"
#include "platform/path.h++"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name())
{
    this->add_flag({"--local"},
                   "Use built-in functions, do not connect to platform service",
                   &this->local);

    this->describe("Software upgrade service tool.");
}

void Options::add_options()
{
    Super::add_options();
    this->add_commands();
}

void Options::initialize()
{
    if (this->local)
    {
        platform::upgrade::native::register_providers();
    }
    else
    {
        platform::upgrade::grpc::register_providers(this->host);
    }
    core::platform::signal_shutdown.connect(
        this->signal_handle,
        std::bind(&Options::deinitialize, this));
}

void Options::deinitialize()
{
    core::platform::signal_shutdown.disconnect(this->signal_handle);
    if (this->local)
    {
        platform::upgrade::native::unregister_providers();
    }
    else
    {
        platform::upgrade::grpc::unregister_providers();
    }
}

void Options::on_monitor_start()
{
    FlagMap flags;
    bool &except = flags["except"];
    bool &show_scan_progress = flags["scan_progress"];
    bool &show_upgrade_available = flags["available"];
    bool &show_upgrade_progress = flags["upgrade_progress"];

    this->get_flags(&flags, false);

    if (!show_upgrade_available && !show_scan_progress && !show_upgrade_progress)
    {
        except = true;
    }

    using namespace std::placeholders;

    if (show_scan_progress != except)
    {
        platform::upgrade::signal_scan_progress.connect(
            this->signal_handle,
            std::bind(&Options::on_scan_progress, this, _1));
    }

    if (show_upgrade_available != except)
    {
        platform::upgrade::signal_upgrade_available.connect(
            this->signal_handle,
            std::bind(&Options::on_upgrade_available, this, _1));
    }

    if (show_upgrade_progress != except)
    {
        platform::upgrade::signal_upgrade_progress.connect(
            this->signal_handle,
            std::bind(&Options::on_upgrade_progress, this, _1));
    }
}

void Options::on_monitor_end()
{
    platform::upgrade::signal_upgrade_progress.disconnect(this->signal_handle);
    platform::upgrade::signal_upgrade_available.disconnect(this->signal_handle);
    platform::upgrade::signal_scan_progress.disconnect(this->signal_handle);
}

void Options::on_scan_progress(
    const platform::upgrade::ScanProgress::Ref &progress)
{
    core::str::format(std::cout,
                      "[%.0s] signal_scan_progress(%s)\n",
                      core::dt::Clock::now(),
                      progress);
}

void Options::on_upgrade_available(
    const platform::upgrade::PackageInfo::Ref &package_info)
{
    core::str::format(std::cout,
                      "[%.0s] signal_upgrade_available(%s)\n",
                      core::dt::Clock::now(),
                      package_info);
}

void Options::on_upgrade_progress(
    const platform::upgrade::UpgradeProgress::Ref &progress)
{
    core::str::format(std::cout,
                      "[%.0s] signal_upgrade_progress(%s)\n",
                      core::dt::Clock::now(),
                      progress);
}

std::unique_ptr<Options> options;
