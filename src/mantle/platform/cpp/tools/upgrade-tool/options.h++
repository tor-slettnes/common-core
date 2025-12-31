// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Upgrade service tool - command line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "upgrade-base.h++"
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

    void initialize() override;
    void deinitialize() override;

    void on_monitor_start() override;
    void on_monitor_end() override;

    void on_scan_progress(
        const upgrade::ScanProgress::ptr &progress);

    void on_upgrade_available(
        const upgrade::PackageInfo::ptr &package_info);

    void on_upgrade_pending(
        const upgrade::PackageInfo::ptr &package_info);

    void on_upgrade_progress(
        const upgrade::UpgradeProgress::ptr &progress);

private:
    void get_current();
    void scan();
    void list_sources();
    void list_available();
    void best_available();
    void install();
    void finalize();

private:
    vfs::Path vfspath(const std::string &path);

private:
    std::string signal_handle;

public:
    bool local;
    std::string command;
    std::vector<std::string> args;

};

extern std::unique_ptr<Options> options;
