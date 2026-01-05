// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief C++ relay - main application
/// @author Tor Slettnes
//==============================================================================

#include "application/init.h++"  // Common init routines
#include "options.h++"           // Command-line options
#include "status/exceptions.h++"
#include "logging/logging.h++"
#include "thread/supervised-thread.h++"

#ifdef USE_ZMQ
#include "relay-zmq-run.h++"
#endif

#ifdef USE_GRPC
#include "relay-grpc-run.h++"
#endif

#include <memory>
#include <thread>

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        core::application::initialize_daemon(argc, argv);

        relay::options = std::make_unique<relay::Options>();
        relay::options->apply(argc, argv);

        std::list<std::thread> server_threads;

#ifdef USE_ZMQ
        if (relay::options->enable_zmq)
        {
            logf_debug("Spawning ZMQ listeners");
            server_threads.push_back(core::thread::supervised_thread(
                relay::zmq::run_zmq_listeners,
                relay::options->zmq_producer_interface,
                relay::options->zmq_consumer_interface));
        }
#endif

#ifdef USE_GRPC
        if (relay::options->enable_grpc)
        {
            logf_debug("Spawning gRPC server");
            server_threads.push_back(core::thread::supervised_thread(
                relay::grpc::run_grpc_service,
                relay::options->bind_address));
        }
#endif

        for (std::thread& t : server_threads)
        {
            t.join();
        }

        return 0;
    }
    catch (...)
    {
        log_critical(std::current_exception());
        return -1;
    }
}
