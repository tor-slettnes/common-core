// -*- c++ -*-
//==============================================================================
/// @file multilogger-zmq-run.c++
/// @brief C++ multilogger - Launch ZeroMQ server implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-zmq-run.h++"
#include "multilogger-zmq-server.h++"
#include "multilogger-zmq-message-writer.h++"
#include "multilogger-zmq-submission-handler.h++"
#include "status/exceptions.h++"


namespace multilogger::zmq
{
    constexpr auto SHUTDOWN_SIGNAL_HANDLE = "multilogger-zmq-service";

    void run_service(
        std::shared_ptr<multilogger::API> api_provider,
        const std::string &bind_address)
    {
        // Instantiate Publisher to relay asynchronous events over ZeroMQ
        auto submission_subscriber = std::make_shared<core::zmq::Subscriber>(
            bind_address,
            SUBMIT_CHANNEL,
            core::zmq::Subscriber::Role::HOST);

        auto submission_handler = multilogger::zmq::SubmissionHandler::create_shared(
            api_provider,
            submission_subscriber);

        auto message_publisher = std::make_shared<core::zmq::Publisher>(
            bind_address,
            MONITOR_CHANNEL,
            core::zmq::Publisher::Role::HOST);

        auto message_writer = multilogger::zmq::MessageWriter::create_shared(
            api_provider,
            message_publisher);

        auto server = multilogger::zmq::Server::create_shared(
            api_provider,
            bind_address);

        //======================================================================
        // Initialize

        message_publisher->initialize();
        message_writer->initialize();
        log_notice("Multilogger ZMQ message publisher is ready on ",
                   message_publisher->address());

        submission_handler->initialize();
        submission_subscriber->initialize();
        log_notice("Multilogger ZMQ submission subscriber is ready on ",
                   submission_subscriber->address());

        server->initialize();
        log_notice("Multilogger ZMQ RPC server is ready on ",
                   server->address());

        //======================================================================
        // Run

        log_debug("Adding ZMQ shutdown handler");
        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                log_info("ZMQ service is shutting down");
                server->stop();
            });


        server->run();

        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);

        //======================================================================
        // Deinitialize

        log_info("Multilogger ZMQ RPC server is shutting down");
        server->deinitialize();

        log_info("Multilogger ZMQ submission subscriber is shutting down");
        submission_subscriber->deinitialize();
        submission_handler->deinitialize();

        log_info("Multilogger ZeroMQ publisher is shutting down");
        message_writer->deinitialize();
        message_publisher->deinitialize();

        log_notice("Multilogger ZeroMQ service is down");

    }

}  // namespace multilogger::zmq
