// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-zmq-client.h++"
#include "demo-provider.h++"
#include "logging/logging.h++"
#include "application/init.h++"

int main(int argc, char** argv)
{
    cc::application::initialize(argc, argv);

    cc::demo::options = std::make_unique<cc::demo::Options>("ZeroMQ");
    cc::demo::options->apply(argc, argv);

    logf_debug("Creating ZeroMQ demo provider");
    cc::demo::provider = cc::demo::zmq::ClientImpl::create_shared(
        cc::demo::options->identity);

    logf_debug("Initializing ZeroMQ demo provider");
    cc::demo::provider->initialize();

    bool success = cc::demo::options->handle_command();

    logf_debug("Deinitializing ZeroMQ demo provider");
    cc::demo::provider->deinitialize();
    cc::demo::provider.reset();

    cc::application::deinitialize();
    return success ? 0 : -1;
}
