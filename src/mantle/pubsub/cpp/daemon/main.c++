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

#include "broker-relay-control.h++"

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
        cc::core::application::initialize_daemon(argc, argv, "relay");

        auto options = std::make_unique<cc::platform::pubsub::Options>();
        options->apply(argc, argv);

        std::list<std::thread> server_threads;

        std::shared_ptr<cc::platform::pubsub::ControlInterface> relay_control;

#ifdef USE_ZMQ
        if (options->enable_zmq)
        {
            logf_debug("Spawning ZMQ listeners");
            server_threads.push_back(cc::core::thread::supervised_thread(
                cc::platform::pubsub::zmq::run_zmq_listeners,
                options->zmq_producer_interface,
                options->zmq_consumer_interface));
        }
#endif

#ifdef USE_GRPC
        if (options->enable_grpc)
        {
            logf_debug("Spawning gRPC server");
            relay_control = cc::platform::pubsub::RelayControl::create_shared();
            server_threads.push_back(cc::core::thread::supervised_thread(
                cc::platform::pubsub::grpc::run_grpc_service,
                relay_control,
                options->bind_address));
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
