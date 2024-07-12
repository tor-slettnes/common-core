// -*- c++ -*-
//==============================================================================
/// @file commands.c++
/// @brief System service control tool - command implementations
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "options.h++"
#include "system-api.h++"
#include "protobuf-message.h++"
#include "string/format.h++"

void Options::add_commands()
{
    this->add_command(
        "get_host_info",
        {},
        "Get general information about the host system",
        std::bind(&Options::get_host_info, this));

    this->add_command(
        "set_host_name",
        {"NAME"},
        "Configure the hostname.",
        std::bind(&Options::set_host_name, this));

    this->add_command(
        "reboot",
        {},
        "Reboot the system",
        std::bind(&Options::reboot, this));

    this->add_command(
        "monitor",
        {"[except]", "[time|time_config|tz_info|tz_config|host_info]", "..."},
        "Stay alive and monitor events. Unless specified, all events are printed.",
        std::bind(&Options::monitor, this));
}

void Options::get_host_info()
{
    std::cout << platform::system::hostconfig->get_host_info()
              << std::endl;
}

void Options::set_host_name()
{
    std::string hostname = this->get_arg("NAME");
    platform::system::hostconfig->set_host_name(hostname);
}

void Options::reboot()
{
    platform::system::hostconfig->reboot();
}

void Options::on_time(const core::dt::TimePoint &tp)
{
    core::str::format(std::cout,
                      "[%.0s] signal_time(%s)\n",
                      core::dt::Clock::now(),
                      tp);
}

void Options::on_time_config(const platform::system::TimeConfig &tc)
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

void Options::on_tz_config(const platform::system::TimeZoneConfig &tc)
{
    core::str::format(std::cout,
                      "[%.0s] signal_tz_config(%s)\n",
                      core::dt::Clock::now(),
                      tc);
}

void Options::on_hostinfo(const platform::system::HostInfo &hi)
{
    core::str::format(std::cout,
                      "[%.0s] signal_hostinfo(%s)\n",
                      core::dt::Clock::now(),
                      hi);
}

void Options::on_productinfo(const platform::system::ProductInfo &pi)
{
    core::str::format(std::cout,
                      "[%.0s] signal_productinfo(%s)\n",
                      core::dt::Clock::now(),
                      pi);
}

void Options::on_monitor_start()
{
    FlagMap flags;
    bool &exclude = flags["except"];
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
        show_time = show_time_config =
            show_tz_info = show_tz_config =
                show_host_info = show_product_info = true;
    }

    using namespace std::placeholders;

    if (show_time != exclude)
    {
        platform::system::signal_time.connect(
            this->signal_handle,
            std::bind(&Options::on_time, this, _1));
    }

    if (show_time_config != exclude)
    {
        platform::system::signal_timeconfig.connect(
            this->signal_handle,
            std::bind(&Options::on_time_config, this, _1));
    }

    if (show_tz_info != exclude)
    {
        platform::system::signal_tzinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_tz_info, this, _1));
    }

    if (show_tz_config != exclude)
    {
        platform::system::signal_tzconfig.connect(
            this->signal_handle,
            std::bind(&Options::on_tz_config, this, _1));
    }

    if (show_host_info != exclude)
    {
        platform::system::signal_hostinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_hostinfo, this, _1));
    }

    if (show_product_info != exclude)
    {
        platform::system::signal_productinfo.connect(
            this->signal_handle,
            std::bind(&Options::on_productinfo, this, _1));
    }
}

void Options::on_monitor_end()
{
    platform::system::signal_productinfo.disconnect(this->signal_handle);
    platform::system::signal_hostinfo.disconnect(this->signal_handle);
    platform::system::signal_tzconfig.disconnect(this->signal_handle);
    platform::system::signal_tzinfo.disconnect(this->signal_handle);
    platform::system::signal_timeconfig.disconnect(this->signal_handle);
    platform::system::signal_time.disconnect(this->signal_handle);
}
