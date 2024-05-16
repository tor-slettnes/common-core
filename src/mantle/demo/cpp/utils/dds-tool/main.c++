// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-dds-client.h++"
#include "demo-provider.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    cc::application::initialize(argc, argv);

    demo::options = std::make_unique<demo::Options>("RTI DDS");
    demo::options->apply(argc, argv);

    logf_debug("Creating DDS demo provider");
    demo::provider = demo::dds::ClientImpl::create_shared(
        demo::options->identity,
        demo::options->domain_id);

    logf_debug("Initializing DDS demo provider");
    demo::provider->initialize();

    bool success = demo::options->handle_command();

    logf_debug("Deinitializing DDS demo provider");
    demo::provider->deinitialize();
    demo::provider.reset();

    cc::application::deinitialize();
    return success ? 0 : -1;
}
