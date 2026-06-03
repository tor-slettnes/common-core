// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Switch control tool - DDS flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "switchboard-dds-subscriber.h++"
#include "switchboard-dds-proxy.h++"
#include "application/init.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    cc::core::application::initialize(argc, argv);

    try
    {
        auto options = std::make_unique<cc::platform::switchboard::Options>("DDS");
        options->apply(argc, argv);

        // Create a switchboard subscriber, which handles both direct publications and RPC responses.
        logf_trace("Creating switchboard Subscriber");
        auto subscriber = cc::platform::switchboard::dds::Subscriber::create_shared(
            options->identity,
            options->domain_id);

        // Create a Switchboard proxy, which makes RPC calls to the real switchboard service.
        logf_trace("Creating Switchboard Proxy");
        auto proxy = cc::platform::switchboard::dds::Proxy::create_shared(
            options->domain_id,
            std::chrono::seconds(options->timeout));

        logf_trace("Initializing switchboard subscriber");
        subscriber->initialize();

        logf_trace("Initializing switchboard proxy");
        proxy->initialize();

        logf_trace("Waiting for service");
        proxy->wait_ready();

        logf_trace("Handling command");
        bool success = options->handle_command(proxy);

        logf_debug("Shutting down proxy");
        proxy->deinitialize();

        logf_debug("Shutting down subscriber");
        subscriber->deinitialize();

        logf_debug("Shutting down application");
        return success ? 0 : -1;
    }
    catch (...)
    {
        std::cout << std::current_exception() << std::endl;
        return -1;
    }
}
