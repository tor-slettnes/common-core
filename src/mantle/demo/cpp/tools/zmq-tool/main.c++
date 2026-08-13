// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "demo-zmq-subscriber.h++"
#include "demo-zmq-client.h++"
#include "demo-provider.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "application/init.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);

    auto options = std::make_unique<cc::demo::Options>("ZeroMQ");
    options->apply(argc, argv);

    auto subscriber = cc::demo::zmq::Subscriber::create_shared(
        options->host);

    cc::demo::provider = cc::demo::zmq::ClientImpl::create_shared(
        options->identity,
        options->host);

    logf_debug("Initializing ZeroMQ demo subscriber");
    subscriber->initialize();
    subscriber->start_listening();

    logf_debug("Initializing ZeroMQ demo provider");
    cc::demo::provider->initialize();

    bool success = options->handle_command();

    // // Invoke shutdown triggers
    // logf_debug("Emitting shutdown signal");
    // core::platform::signal_shutdown.emit();

    logf_debug("Deinitializing ZeroMQ demo provider");
    cc::demo::provider->deinitialize();

    logf_debug("Deinitializing ZeroMQ demo subscriber");
    subscriber->stop_listening();
    subscriber->deinitialize();

    // Invoke shutdown triggers
    logf_debug("Emitting shutdown signal");
    cc::core::platform::signal_shutdown.emit();

    logf_debug("Deinitialized ZeroMQ demo");

    return success ? 0 : -1;
}
