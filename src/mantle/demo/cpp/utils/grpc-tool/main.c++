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
    core::application::initialize(argc, argv);

    demo::options = std::make_unique<demo::Options>("gRPC");
    demo::options->apply(argc, argv);

    logf_debug("Creating gRPC demo provider");
    demo::provider = demo::grpc::ClientImpl::create_shared(
        demo::options->identity,
        demo::options->host);

    logf_debug("Initializing gRPC demo provider");
    demo::provider->initialize();

    logf_debug("Handling command in gRPC demo provider");
    bool success = demo::options->handle_command();

    logf_debug("Deinitializing gRPC demo provider");
    demo::provider->deinitialize();

    return success ? 0 : -1;
}
