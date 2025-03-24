// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief VFS service control tool - command line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "vfs-base.h++"
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

    void on_context(
        core::signal::MappingAction action,
        const std::string &key,
        const vfs::Context::ptr &cxt);

    void on_context_in_use(
        core::signal::MappingAction action,
        const std::string &key,
        const vfs::Context::ptr &cxt);

private:
    void get_contexts();
    void get_context();
    void open_context();
    void close_context();
    void get_volume_info();
    void get_file_info();
    void get_dir();
    void list();
    void locate();
    void copy();
    void move();
    void remove();
    void mkdir();
    void setattr();
    void getattrs();
    void clearattr();
    void download();
    void upload();

private:
    std::string get_context_arg(const std::string &what = "context name");
    vfs::Path get_vfspath_arg(const std::string &what = "VFS path");
    vfs::Path vfspath(const std::string &path);

private:
    std::string signal_handle;

public:
    bool local;
    std::string command;
    std::vector<std::string> args;

};

extern std::unique_ptr<Options> options;
