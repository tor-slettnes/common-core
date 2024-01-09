// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "implementations.h++"
#include "argparse/server.h++"
#include "dds-options-server.h++"

namespace cc::demo
{
    // @brief Command-line options for demo service
    ///
    // @note
    //    We inherit from `cc::argparse::DDSServiceOptions` in order to
    //    get DDS logging capabilities. To build without DDS logging, we could
    //    inherit directly from `cc::argparse::ServerOptions`.

    class Options : public cc::argparse::DDSServiceOptions
    {
        using Super = cc::argparse::DDSServiceOptions;

    public:
        Options();

        void add_options() override;

    public:
        bool enable_grpc;
        bool enable_dds;
    };

    extern std::unique_ptr<Options> options;
}  // namespace cc::demo
