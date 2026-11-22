// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Secrets tool - command line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/command.h++"

namespace cc::core::platform
{
    class Options : public core::argparse::CommandOptions
    {
        using Super = core::argparse::CommandOptions;

    public:
        Options();

    private:
        void add_options() override;
        void add_commands();

    private:
        void showkey();
        void encrypt();
        void decrypt();
    };
}  // namespace cc::platform::vfs
