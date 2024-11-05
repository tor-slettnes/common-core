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
    return ::options->handle_command();
}
