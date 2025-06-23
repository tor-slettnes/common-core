// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-run.c++
/// @brief C++ demo - Launch ZeroMQ server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-run.h++"
#include "demo-zmq-publisher.h++"
#include "demo-zmq-server.h++"
#include "status/exceptions.h++"

namespace demo::zmq
{
    constexpr auto SHUTDOWN_SIGNAL_HANDLE = "demo-zmq-service";

    void run_zmq_service(
        std::shared_ptr<demo::API> api_provider,
        const std::string &bind_address)
    {
        // Instantiate Publisher to relay asynchronous events over ZeroMQ
        auto zmq_publisher = demo::zmq::Publisher::create_shared(
            bind_address);

        // Instantiate Server to handle incoming requests from client
        auto zmq_server = demo::zmq::Server::create_shared(
            api_provider,
            bind_address);

        //======================================================================
        // Initialize

        zmq_publisher->initialize();
        log_notice("Demo ZeroMQ publisher is ready on ", zmq_publisher->address());

        zmq_server->initialize();
        log_notice("Demo ZeroMQ command server is ready on ", zmq_server->address());


        //======================================================================
        // Run

        log_debug("Adding ZMQ shutdown handler");
        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                log_info("ZMQ service is shutting down");
                zmq_server->stop();
            });


        zmq_server->run();

        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);

        //======================================================================
        // Deinitialize

        log_notice("Demo ZeroMQ command server is shutting down");
        zmq_server->deinitialize();

        log_notice("Demo ZeroMQ publisher is shutting down");
        zmq_publisher->deinitialize();
    }

}  // namespace demo::zmq
