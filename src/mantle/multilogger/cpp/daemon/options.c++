// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"

Options::Options()
{
    this->describe("Log server.");
}

void Options::add_options()
{
    Super::add_options();
}

std::unique_ptr<Options> options;
