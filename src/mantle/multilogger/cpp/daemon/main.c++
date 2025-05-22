// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Log Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "grpc-service.h++"
#include "multilogger-native.h++"
#include "application/init.h++"
#include "status/exceptions.h++"

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        core::application::initialize_daemon(argc, argv);
        ::options = std::make_unique<Options>();
        ::options->apply(argc, argv);

        auto log_provider = multilogger::native::Logger::create_shared();

        log_provider->initialize();
        multilogger::grpc::run_service(log_provider);
        log_provider->deinitialize();
        return 0;
    }
    catch (...)
    {
        std::cerr << std::current_exception() << std::endl;
        // log_critical(std::current_exception());
        return -1;
    }
}
