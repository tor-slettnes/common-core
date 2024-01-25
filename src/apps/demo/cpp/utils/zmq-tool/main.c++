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
    cc::application::initialize(argc, argv);

    cc::demo::options = std::make_unique<cc::demo::Options>("ZeroMQ");
    cc::demo::options->apply(argc, argv);

    auto subscriber = cc::demo::zmq::Subscriber::create_shared(
        cc::demo::options->host);

    cc::demo::provider = cc::demo::zmq::ClientImpl::create_shared(
        cc::demo::options->identity,
        cc::demo::options->host);

    logf_debug("Initializing ZeroMQ demo subscriber");
    subscriber->initialize();

    logf_debug("Initializing ZeroMQ demo provider");
    cc::demo::provider->initialize();

    bool success = cc::demo::options->handle_command();

    logf_debug("Deinitializing ZeroMQ demo provider");
    cc::demo::provider->deinitialize();
    cc::demo::provider.reset();

    logf_debug("Deinitializing ZeroMQ demo subscriber");
    subscriber->deinitialize();
    subscriber.reset();

    cc::application::deinitialize();
    return success ? 0 : -1;
}
