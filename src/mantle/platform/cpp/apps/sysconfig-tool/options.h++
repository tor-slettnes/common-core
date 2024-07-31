// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief SysConfig service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "sysconfig.h++"
#include "argparse/command.h++"

class Options : public core::argparse::CommandOptions
{
    using Super = core::argparse::CommandOptions;

public:
    Options();

    using Super::handle_command;

private:
    void add_options() override;
    void add_commands();

    void initialize();
    void deinitialize();

    bool handle_command(const Handler &handler) override;

    void on_monitor_start() override;
    void on_monitor_end() override;

    void on_time(const core::dt::TimePoint &tp);
    void on_time_config(const platform::sysconfig::TimeConfig &tc);
    void on_tz_info(const core::dt::TimeZoneInfo &ti);
    void on_tz_config(const platform::sysconfig::TimeZoneConfig &tc);
    void on_hostinfo(const platform::sysconfig::HostInfo &hi);
    void on_productinfo(const platform::sysconfig::ProductInfo &pi);

    void get_time();
    void set_time();
    void set_ntp();
    void get_ntp();
    void list_timezone_areas();
    void list_timezone_countries();
    void list_timezone_specs();
    void get_timezone_spec();
    void set_timezone();
    void get_timezone_config();
    void get_timezone_info();
    void get_host_info();
    void set_host_name();
    void reboot();

private:
    std::string signal_handle;
};

extern std::unique_ptr<Options> options;
