// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

#if USE_DDS
#include "dds-options-server.h++"
using OptionsBase = shared::dds::ServerOptions;

#else
#include "argparse/server.h++"
using OptionsBase = shared::argparse::ServerOptions;

#endif

namespace demo
{
    // @brief Command-line options for demo service
    ///
    // @note
    //    We inherit from `shared::argparse::DDSServiceOptions` in order to
    //    get DDS logging capabilities. To build without DDS logging, we could
    //    inherit directly from `shared::argparse::ServerOptions`.

    class Options : public OptionsBase
    {
        using Super = OptionsBase;

    public:
        Options();

        void add_options() override;

    public:
        bool enable_grpc;
        bool enable_dds;
        bool enable_zmq;
    };

    extern std::unique_ptr<Options> options;
}  // namespace demo
