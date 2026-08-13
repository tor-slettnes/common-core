// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Switch control tool - gPRC flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "switchboard-grpc-proxy.h++"
#include "application/init.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);
    try
    {
        auto options = std::make_unique<cc::platform::switchboard::Options>("gRPC");
        options->apply(argc, argv);

        // Create a Switchboard proxy, which makes RPC calls to the real switchboard service.
        logf_trace("Creating Switchboard Proxy");
        auto proxy = cc::platform::switchboard::grpc::Proxy::create_shared(
            options->host);

        logf_trace("Initializing switchboard proxy");
        proxy->initialize();

        logf_trace("Waiting for service");
        proxy->wait_ready();

        logf_trace("Handling command");
        bool success = options->handle_command(proxy);

        logf_debug("Shutting down proxy");
        proxy->deinitialize();

        logf_debug("Shutting down application");
        return success ? 0 : -1;
    }
    catch (...)
    {
        std::cout << std::current_exception() << std::endl;
        return -1;
    }
}
