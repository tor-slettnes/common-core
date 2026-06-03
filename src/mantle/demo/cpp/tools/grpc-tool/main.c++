// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Demo service control tool - gRPC flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "demo-grpc-client.h++"
#include "demo-provider.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);

    try
    {
        ::options = std::make_unique<cc::demo::Options>("gRPC");
        ::options->apply(argc, argv);

        logf_debug("Creating gRPC demo provider");
        cc::demo::provider = cc::demo::grpc::ClientImpl::create_shared(
            ::options->identity,
            ::options->host);

        logf_debug("Initializing gRPC demo provider");
        cc::demo::provider->initialize();

        logf_debug("Handling command in gRPC demo provider");
        bool success = ::options->handle_command();

        logf_debug("Deinitializing gRPC demo provider");
        cc::demo::provider->deinitialize();
        return success ? 0 : -1;
    }
    catch (...)
    {
        std::cerr << std::current_exception() << std::endl;
        return -1;
    }

}
