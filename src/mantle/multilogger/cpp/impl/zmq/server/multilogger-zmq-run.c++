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

        server->run();

        //======================================================================
        // Deinitialize

        log_notice("Multilogger ZMQ RPC server is shutting down");
        server->deinitialize();

        log_notice("Multilogger ZMQ submission subscriber is shutting down");
        submission_subscriber->deinitialize();
        submission_handler->deinitialize();

        log_notice("Multilogger ZeroMQ message writer is shutting down");
        message_writer->deinitialize();

        std::cout << "Multilogger ZeroMQ publisher is shutting down" << std::endl;
        log_notice("Multilogger ZeroMQ publisher is shutting down");
        message_publisher->deinitialize();

        log_notice("Multilogger ZeroMQ service is down");

    }

}  // namespace multilogger::zmq
