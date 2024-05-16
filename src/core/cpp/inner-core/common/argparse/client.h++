// -*- c++ -*-
//==============================================================================
/// @file client.h++
/// @brief Parse commmand line options for client applications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "common.h++"

namespace core::argparse
{
    //==========================================================================
    /// Option parser for client apps
    class ClientOptions : public CommonOptions
    {
        using Super = CommonOptions;

    protected:
        ClientOptions();

    public:
        void add_options() override;

    public:
        std::string host;
        uint timeout;
    };
}  // namespace core::argparse
