// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Parse commmand line options
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "argparse/server.h++"

namespace pubsub
{
    class Options : public core::argparse::ServerOptions
    {
        using This = Options;
        using Super = core::argparse::ServerOptions;

    public:
        Options();

        void add_options() override;

    public:
        bool enable_grpc;
        bool enable_zmq;

        std::string zmq_producer_interface;
        std::string zmq_consumer_interface;
    };

    extern std::unique_ptr<Options> options;
}  // namespace pubsub
