// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/server.h++"

namespace switchboard
{
    class Options : public core::argparse::ServerOptions
    {
        using Super = core::argparse::ServerOptions;

    public:
        Options();

        void add_options() override;

    public:
        bool enable_grpc;
        bool enable_dds;
    };

    extern std::unique_ptr<Options> options;
}  // namespace switchboard
