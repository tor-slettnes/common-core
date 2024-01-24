// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - gRPC flavor
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "demo-grpc-client.h++"
#include "demo-provider.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    cc::application::initialize(argc, argv);

    cc::demo::options = std::make_unique<cc::demo::Options>("gRPC");
    cc::demo::options->apply(argc, argv);

    logf_debug("Creating gRPC demo provider");
    cc::demo::provider = cc::demo::grpc::ClientImpl::create_shared(
        cc::demo::options->identity);

    logf_debug("Initializing gRPC demo provider");
    cc::demo::provider->initialize();

    logf_debug("Handling command in gRPC demo provider");
    bool success = cc::demo::options->handle_command();

    logf_debug("Deinitializing gRPC demo provider");
    cc::demo::provider->deinitialize();

    cc::application::deinitialize();
    return success ? 0 : -1;
}
