// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief System Control Service main application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "options.h++"
#include "switchboard-central.h++"
#include "application/init.h++"
#include "thread/supervised-thread.h++"

#ifdef USE_DDS
#include "switchboard-dds-run.h++"
#endif

#ifdef USE_GRPC
#include "switchboard-grpc-run.h++"
#endif

int main(int argc, char** argv)
{
    try
    {
        core::application::initialize_daemon(argc, argv);

        switchboard::options = std::make_unique<switchboard::Options>();
        switchboard::options->apply(argc, argv);

        // Prepare switchboard request handler
        auto switchboard_provider = switchboard::Central::create_shared();
        logf_debug("Initializing Switchboard provider: %s",
                   switchboard_provider->implementation());
        switchboard_provider->initialize();

        std::list<std::thread> server_threads;

#ifdef USE_GRPC
        if (switchboard::options->enable_grpc)
        {
            logf_debug("Starting gRPC server");
            server_threads.push_back(
                core::thread::supervised_thread(
                    switchboard::grpc::run_grpc_service,
                    switchboard_provider,
                    switchboard::options->bind_address));
        }
#endif

#ifdef USE_DDS
        if (switchboard::options->enable_dds)
        {
            logf_debug("Starting DDS server");
            server_threads.push_back(
                core::thread::supervised_thread(
                    switchboard::dds::run_dds_service,
                    switchboard_provider,
                    switchboard::options->identity,
                    switchboard::options->domain_id));
        }
#endif

        for (std::thread& t : server_threads)
        {
            t.join();
        }

        logf_debug("Deinitializing Switchboard provider: %s",
                   switchboard_provider->implementation());
        switchboard_provider->deinitialize();
        return 0;
    }
    catch (...)
    {
        log_critical(std::current_exception());
        return -1;
    }
}
