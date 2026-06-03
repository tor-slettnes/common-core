// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/server.h++"

namespace cc::platform::switchboard
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

}  // namespace cc::platform::switchboard

extern std::unique_ptr<cc::platform::switchboard::Options> options;
