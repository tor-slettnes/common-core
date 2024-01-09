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

    cc::demo::options = std::make_unique<cc::demo::Options>("RTI DDS");
    cc::demo::options->apply(argc, argv);

    logf_debug("Creating DDS demo provider");
    cc::demo::provider = cc::demo::dds::ClientImpl::create_shared(
        cc::demo::options->identity,
        cc::demo::options->domain_id);

    logf_debug("Initializing DDS demo provider");
    cc::demo::provider->initialize();

    bool success = cc::demo::options->handle_command();

    logf_debug("Deinitializing DDS demo provider");
    cc::demo::provider->deinitialize();
    cc::demo::provider.reset();

    cc::application::deinitialize();
    return success ? 0 : -1;
}
