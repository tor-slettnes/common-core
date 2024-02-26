// -*- c++ -*-
//==============================================================================
/// @file client.c++
/// @brief Parse commmand line options for client applications
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "client.h++"

namespace core::argparse
{
    //==========================================================================
    // ClientOptions methods

    ClientOptions::ClientOptions()
        : Super(false)
    {
    }

    void ClientOptions::add_options()
    {
        Super::add_options();
        this->add_opt(
            {"-h", "--host"},
            "ADDRESS",
            "Server address in the form [PERSONALITY@][HOST][:PORT], "
            "where HOST and PORT, if specified, take precedence. ",
            &this->host);

        this->add_opt<uint>(
            {"--timeout"},
            "SECONDS",
            "Exit with an error if server is not available after the specified time (default: %default%)",
            &this->timeout,
            30);
    }

}  // namespace core::argparse
