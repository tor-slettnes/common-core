// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Avro utility - command line options
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

public:
    bool local;
    std::string command;
    std::vector<std::string> args;
};

extern std::unique_ptr<Options> options;
