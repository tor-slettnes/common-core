// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Switch control tool - DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "switchboard-dds-subscriber.h++"
#include "switchboard-dds-proxy.h++"
#include "application/init.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    try
    {
        switchboard::options = std::make_unique<switchboard::Options>("DDS");
        switchboard::options->apply(argc, argv);

        // Create a switchboard subscriber, which handles both direct publications and RPC responses.
        logf_trace("Creating switchboard Subscriber");
        auto subscriber = switchboard::dds::Subscriber::create_shared(
            switchboard::options->identity,
            switchboard::options->domain_id);

        // Create a Switchboard proxy, which makes RPC calls to the real switchboard service.
        logf_trace("Creating Switchboard Proxy");
        auto proxy = switchboard::dds::Proxy::create_shared(
            switchboard::options->domain_id,
            std::chrono::seconds(switchboard::options->timeout));

        logf_trace("Initializing switchboard subscriber");
        subscriber->initialize();

        logf_trace("Initializing switchboard proxy");
        proxy->initialize();

        logf_trace("Waiting for service");
        proxy->wait_ready();

        logf_trace("Handling command");
        bool success = switchboard::options->handle_command(proxy);

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
