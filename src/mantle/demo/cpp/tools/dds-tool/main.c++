// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - RTI DDS flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "demo-dds-client.h++"
#include "demo-provider.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);

    auto options = std::make_unique<cc::demo::Options>("RTI DDS");
    options->apply(argc, argv);

    logf_debug("Creating DDS demo provider");
    cc::demo::provider = cc::demo::dds::ClientImpl::create_shared(
        options->identity,
        options->domain_id);

    logf_debug("Initializing DDS demo provider");
    cc::demo::provider->initialize();

    bool success = options->handle_command();

    logf_debug("Deinitializing DDS demo provider");
    cc::demo::provider->deinitialize();
    cc::demo::provider.reset();

    return success ? 0 : -1;
}
