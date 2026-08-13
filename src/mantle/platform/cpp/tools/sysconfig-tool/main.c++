// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief SysConfig service control tool - main
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "application/init.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);

    auto options = std::make_unique<cc::platform::sysconfig::Options>();
    options->apply(argc, argv);

    return options->handle_command() ? 0 : 1;
}
