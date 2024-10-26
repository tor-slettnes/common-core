// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Log tool - gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "logger-grpc-client.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char** argv)
{
    core::application::initialize(argc, argv);

    ::options = std::make_unique<::Options>();
    ::options->apply(argc, argv);

    auto client = logger::grpc::ClientImpl::create_shared(
        ::options->identity,
        ::options->host);

    client->initialize();
    bool success = ::options->handle_command(client);
    ::client->deinitialize();

    return success ? 0 : -1;
}
