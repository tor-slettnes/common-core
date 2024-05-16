// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief C++ demo - main application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "application/init.h++"  // Common init routines
#include "options.h++"           // Command-line options
#include "demo-native.h++"       // Native Demo API implementation
#include "status/exceptions.h++"
#include "thread/supervised_thread.h++"

#ifdef USE_DDS
#include "demo-dds-run.h++"  // DDS server implementation
#endif

#ifdef USE_GRPC
#include "demo-grpc-run.h++"  // gRPC server implementation
#endif

#ifdef USE_ZMQ
#include "demo-zmq-run.h++"  // ZeroMQ+ProtoBuf server implementation
#endif

#include <memory>
#include <thread>

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        cc::application::initialize(argc, argv);

        demo::options = std::make_unique<demo::Options>();
        demo::options->apply(argc, argv);

        // API provider. In this process we use the native/direct implementation.
        auto api_provider = demo::NativeImpl::create_shared(
            demo::options->identity);

        logf_debug("Initializing Demo API provider: %s", api_provider->implementation());
        api_provider->initialize();

        std::list<std::thread> server_threads;

#ifdef USE_GRPC
        if (demo::options->enable_grpc)
        {
            logf_debug("Spawning gRPC server");
            server_threads.push_back(cc::thread::supervised_thread(
                demo::grpc::run_grpc_service,
                api_provider,
                ""));
        }
#endif

#ifdef USE_DDS
        if (demo::options->enable_dds)
        {
            logf_debug("Spawning DDS server");
            server_threads.push_back(cc::thread::supervised_thread(
                demo::dds::run_dds_service,
                api_provider,
                demo::options->identity,
                demo::options->domain_id));
        }
#endif

#ifdef USE_ZMQ
        if (demo::options->enable_zmq)
        {
            log_debug("Spawning ZMQ server");
            server_threads.push_back(cc::thread::supervised_thread(
                demo::zmq::run_zmq_service,
                api_provider,
                ""));
        }
#endif

        for (std::thread& t : server_threads)
        {
            t.join();
        }

        logf_debug("Deinitializing DEMO API provider: %s", api_provider->implementation());
        api_provider->deinitialize();
        return 0;
    }
    catch (...)
    {
        log_critical(std::current_exception());
        return -1;
    }
}
