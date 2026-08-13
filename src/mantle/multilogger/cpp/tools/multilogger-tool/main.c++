// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Log tool - gRPC client
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "multilogger-grpc-client.h++"
#include "application/init.h++"
#include "logging/logging.h++"

int main(int argc, char **argv)
{
    cc::core::application::initialize(argc, argv);
    auto options = std::make_unique<cc::platform::multilogger::Options>();
    options->apply(argc, argv);
    return options->handle_command();
}
