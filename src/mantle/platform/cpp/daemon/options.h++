// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/server.h++"

namespace cc::platform
{
    class Options : public core::argparse::ServerOptions
    {
        using Super = core::argparse::ServerOptions;

    public:
        Options();

        void add_options() override;

    public:
        std::string host;
    };
}  // namespace cc::platform

extern std::unique_ptr<cc::platform::Options> options;
