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

    void on_monitor_start() override;
    void on_monitor_end() override;

    void log();
    void define_contract();
    void get_contract();
    void list_contracts();
};

extern std::unique_ptr<Options> options;
