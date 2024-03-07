/// -*- c++ -*-
//==============================================================================
/// @file dds-options-server.h++
/// @brief An option parser with DDS specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "argparse/server.h++"

namespace core::dds
{
    class ServerOptions : public argparse::ServerOptions
    {
        using This = ServerOptions;
        using Super = argparse::ServerOptions;

    public:
        using Super::Super;

        void add_log_options() override;
        void register_loggers() override;

    private:
        bool log_to_dds;
        bool log_to_dl;
    };

}  // namespace core::argparse
