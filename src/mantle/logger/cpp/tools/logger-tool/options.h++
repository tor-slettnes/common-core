// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for log tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "logger-api.h++"

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
    void on_log_event(core::status::Event::ptr event);

    void submit();
    void add_sink();
    void remove_sink();
    void get_sink();
    void list_sinks();
    void list_static_fields();

public:
    std::shared_ptr<logger::API> provider;

private:
    std::string signal_handle;
};

extern std::unique_ptr<Options> options;
