// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief SysConfig service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "sysconfig-providers-grpc.h++"
#include "platform/path.h++"
#include "protobuf-message.h++"
#include "buildinfo.h"

Options::Options()
    : Super(),
      signal_handle(core::platform::path->exec_name())
{
    this->describe("Command-line tool for SysConfig gRPC service.");
}

void Options::add_options()
{
    Super::add_options();
    this->add_commands();
}

void Options::initialize()
{
    logf_info("Creating SysConfig client: %s");
    platform::sysconfig::grpc::register_providers(this->host);
    core::platform::signal_shutdown.connect(
        this->signal_handle,
        std::bind(&Options::deinitialize, this));
}

void Options::deinitialize()
{
    core::platform::signal_shutdown.disconnect(this->signal_handle);
    logf_info("Shutting down SysConfig client");
    platform::sysconfig::grpc::unregister_providers();
}

bool Options::handle_command(const Handler &handler)
{
    this->initialize();
    bool result = Super::handle_command(handler);
    this->deinitialize();
    return result;
}

void Options::on_monitor_start()
{
    FlagMap flags;
    bool &except = flags["except"];
    bool &show_time = flags["time"];
    bool &show_time_config = flags["time_config"];
    bool &show_tz_info = flags["tz_info"];
    bool &show_tz_config = flags["tz_config"];
    bool &show_host_info = flags["host_info"];
    bool &show_product_info = flags["product_info"];
    this->get_flags(&flags, false);

    if (!show_time && !show_time_config &&
        !show_tz_info && !show_tz_config &&
        !show_host_info && !show_product_info)
    {
        except = true;
    }

    using namespace std::placeholders;

    if (show_time != except)
    {
        platform::sysconfig::signal_time.connect(
            this->signal_handle,
            std::bind(&Options::on_time, this, _1));
    }

    if (show_time_config != except)
    {
        platform::sysconfig::signal_timeconfig.connect(
            this->signal_handle,
            std::bind(&Options::on_time_config, this, _1));
    }

    if (show_tz_info != except)
    {
        platform::sysconfig::signal_tzinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_tz_info, this, _1));
    }

    if (show_tz_config != except)
    {
        platform::sysconfig::signal_tzconfig.connect(
            this->signal_handle,
            std::bind(&Options::on_tz_config, this, _1));
    }

    if (show_host_info != except)
    {
        platform::sysconfig::signal_hostinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_hostinfo, this, _1));
    }

    if (show_product_info != except)
    {
        platform::sysconfig::signal_productinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_productinfo, this, _1));
    }
}

void Options::on_monitor_end()
{
    platform::sysconfig::signal_productinfo.disconnect(this->signal_handle);
    platform::sysconfig::signal_hostinfo.disconnect(this->signal_handle);
    platform::sysconfig::signal_tzconfig.disconnect(this->signal_handle);
    platform::sysconfig::signal_tzinfo.disconnect(this->signal_handle);
    platform::sysconfig::signal_timeconfig.disconnect(this->signal_handle);
    platform::sysconfig::signal_time.disconnect(this->signal_handle);
}

void Options::on_time(const core::dt::TimePoint &tp)
{
    core::str::format(std::cout,
                      "[%.0s] signal_time(%s)\n",
                      core::dt::Clock::now(),
                      tp);
}

void Options::on_time_config(const platform::sysconfig::TimeConfig &tc)
{
    core::str::format(std::cout,
                      "[%.0s] signal_time_config(%s)\n",
                      core::dt::Clock::now(),
                      tc);
}

void Options::on_tz_info(const core::dt::TimeZoneInfo &ti)
{
    core::str::format(std::cout,
                      "[%.0s] signal_tz_info(%s)\n",
                      core::dt::Clock::now(),
                      ti);
}

void Options::on_tz_config(const platform::sysconfig::TimeZoneCanonicalName &zone)
{
    core::str::format(std::cout,
                      "[%.0s] signal_tz_config(%s)\n",
                      core::dt::Clock::now(),
                      zone);
}

void Options::on_hostinfo(const platform::sysconfig::HostInfo &hi)
{
    core::str::format(std::cout,
                      "[%.0s] signal_hostinfo(%s)\n",
                      core::dt::Clock::now(),
                      hi);
}

void Options::on_productinfo(const platform::sysconfig::ProductInfo &pi)
{
    core::str::format(std::cout,
                      "[%.0s] signal_productinfo(%s)\n",
                      core::dt::Clock::now(),
                      pi);
}

std::unique_ptr<Options> options;
