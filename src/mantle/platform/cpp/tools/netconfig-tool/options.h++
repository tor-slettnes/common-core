// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief NetConfig service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "netconfig.h++"
#include "argparse/command.h++"

class Options : public core::argparse::CommandOptions
{
    using Super = core::argparse::CommandOptions;

public:
    Options();

private:
    void add_options() override;
    void add_commands();

    void initialize() override;
    void deinitialize() override;

    void on_monitor_start() override;
    void on_monitor_end() override;

    void on_globaldata(
        const netconfig::GlobalData::ptr &data);

    void on_connection(
        core::signal::MappingAction action,
        const std::string &key,
        netconfig::ConnectionData::ptr data);

    void on_active_connection(
        core::signal::MappingAction action,
        const std::string &key,
        netconfig::ActiveConnectionData::ptr data);

    void on_accesspoint(
        core::signal::MappingAction action,
        const std::string &key,
        netconfig::AccessPointData::ptr data);

    void on_device(
        core::signal::MappingAction action,
        const std::string &key,
        netconfig::DeviceData::ptr data);

private:
    void get_state();
    void get_devices();
    void get_connections();
    void define_connection();
    void remove_connection();
    void activate_connection();
    void get_active_connections();
    void request_scan();
    void get_aps();
    void connect_ap();
    void set_wireless_allowed();
    void set_wireless_enabled();

private:
    std::string signal_handle;

public:
    bool local;
    std::string command;
    std::vector<std::string> args;
};

extern std::unique_ptr<Options> options;
