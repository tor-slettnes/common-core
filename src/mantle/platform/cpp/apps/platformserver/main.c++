// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Platform Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"                  // Command-line options
#include "platform-grpc-server.h++"     // Load gRPC server
#include "system-providers-native.h++"  // System services
#include "application/init.h++"         // Common init routines
#include "status/exceptions.h++"

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        core::application::initialize(argc, argv);
        ::options = std::make_unique<Options>();
        ::options->apply(argc, argv);

        platform::system::native::register_providers();
        platform::run_grpc_service();
        platform::system::native::unregister_providers();
        return 0;
    }
    catch (...)
    {
        log_critical(std::current_exception());
        return -1;
    }
}
