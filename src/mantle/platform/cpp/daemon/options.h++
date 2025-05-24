// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/server.h++"

class Options : public core::argparse::ServerOptions
{
    using Super = core::argparse::ServerOptions;

public:
    Options();

    void add_options() override;

public:
    std::string host;
};

extern std::unique_ptr<Options> options;
