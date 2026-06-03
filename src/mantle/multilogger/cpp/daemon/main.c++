// -*- c++ -*-
//==============================================================================
/// @file main.c++
/// @brief Log Server
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"
#include "multilogger-native.h++"
#include "application/init.h++"
#include "logging/logging.h++"
#include "status/exceptions.h++"
#include "thread/supervised-thread.h++"

#ifdef USE_GRPC
#include "multilogger-grpc-run.h++"
#endif

#ifdef USE_ZMQ
#include "multilogger-zmq-run.h++"
#endif

int main(int argc, char** argv)
{
    try
    {
        // Initialize paths, load settings, set up shutdown signal handlers
        cc::core::application::initialize_daemon(argc, argv, "multilogger");
        ::options = std::make_unique<cc::platform::multilogger::Options>();
        ::options->apply(argc, argv);

        auto log_provider = cc::platform::multilogger::native::Logger::create_shared();
        log_provider->initialize();

        std::list<std::thread> server_threads;

#ifdef USE_GRPC
        if (::options->enable_grpc)
        {
            logf_debug("Spawning gRPC server");
            server_threads.push_back(cc::core::thread::supervised_thread(
                cc::platform::multilogger::grpc::run_service,
                log_provider,
                ::options->bind_address));
        }
#endif

#ifdef USE_ZMQ
        if (::options->enable_zmq)
        {
            logf_debug("Spawning ZeroMQ server");
            server_threads.push_back(cc::core::thread::supervised_thread(
                cc::platform::multilogger::zmq::run_service,
                log_provider,
                ::options->bind_address));
        }
#endif

        for (std::thread& t : server_threads)
        {
            t.join();
        }

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
