// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-zmq-subscriber.h++"
#include "demo-zmq-client.h++"
#include "demo-provider.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "application/init.h++"

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    demo::options = std::make_unique<demo::Options>("ZeroMQ");
    demo::options->apply(argc, argv);

    auto subscriber = demo::zmq::Subscriber::create_shared(
        demo::options->host);

    demo::provider = demo::zmq::ClientImpl::create_shared(
        demo::options->identity,
        demo::options->host);

    logf_debug("Initializing ZeroMQ demo subscriber");
    subscriber->initialize();

    logf_debug("Initializing ZeroMQ demo provider");
    demo::provider->initialize();

    bool success = demo::options->handle_command();

    // Invoke shutdown triggers
    logf_debug("Emitting shutdown signal");
    core::platform::signal_shutdown.emit();

    logf_debug("Deinitializing ZeroMQ demo provider");
    demo::provider->deinitialize();
    demo::provider.reset();

    logf_debug("Deinitializing ZeroMQ demo subscriber");
    subscriber->deinitialize();
    subscriber.reset();

    logf_debug("Deinitializing application");
    core::application::deinitialize();
    return success ? 0 : -1;
}
