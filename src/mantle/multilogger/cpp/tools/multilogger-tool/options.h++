// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for log tool
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/command.h++"
#include "multilogger-api.h++"

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

    void open_stream_sink(core::status::Level threshold);
    void close_stream_sink();
    void on_log_item(core::types::Loggable::ptr item);

    void submit();
    void add_sink();
    void remove_sink();
    void get_sink();
    void get_all_sinks();
    void list_sinks();
    void list_message_fields();
    void list_error_fields();

public:
    std::shared_ptr<multilogger::API> provider;
    std::shared_ptr<core::logging::Sink> stream_sink;

private:
    std::string signal_handle;
};

extern std::unique_ptr<Options> options;
