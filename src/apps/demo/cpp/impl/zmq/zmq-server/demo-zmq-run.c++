// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-run.c++
/// @brief C++ demo - Launch ZeroMQ server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-zmq-run.h++"
#include "demo-zmq-publisher.h++"
#include "demo-zmq-server.h++"

namespace cc::demo::zmq
{
    void run_zmq_service(
        std::shared_ptr<cc::demo::API> api_provider,
        const std::string &bind_address)
    {
        // Instantiate Publisher to relay asynchronous events over ZeroMQ
        auto zmq_publisher = cc::demo::zmq::Publisher::create_shared(
            bind_address);

        // Instantiate Server to handle incoming requests from client
        auto zmq_server = cc::demo::zmq::Server::create_shared(
            api_provider,
            bind_address);

        //======================================================================
        // Initialize

        try
        {
            log_debug("Initializing Demo ZeroMQ publisher");
            zmq_publisher->initialize();
            log_info("Demo ZeroMQ publisher is ready on ", zmq_publisher->bind_address());

            log_debug("Initializing Demo ZeroMQ command server");
            zmq_server->initialize();
            log_info("Demo ZeroMQ command server is ready on ", zmq_server->bind_address());
            zmq_server->run();
            log_info("Demo ZeroMQ command server is shutting down");

            log_debug("Deinitializing Demo ZeroMQ server");
            zmq_server->deinitialize();

            log_debug("Deinitializing Demo ZeroMQ publisher");
            zmq_publisher->deinitialize();
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << std::endl;
            throw;
        }
    }

}  // namespace cc::demo::zmq
