// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Relay service control tool - gRPC flavor
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "application/init.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"

int main(int argc, char **argv)
{
    core::application::initialize(argc, argv);
    auto options = std::make_unique<relay::grpc::Options>();
    options->apply(argc, argv);

    try
    {
        return options->handle_command();
    }
    catch (...)
    {
        std::cerr << std::current_exception() << std::endl;
        return -1;
    }

}
