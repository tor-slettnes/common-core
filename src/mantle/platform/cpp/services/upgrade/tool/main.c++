// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Upgrade service tool - main
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "application/init.h++"

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    ::options = std::make_unique<::Options>();
    ::options->apply(argc, argv);

    return ::options->handle_command() ? 0 : 1;
}
