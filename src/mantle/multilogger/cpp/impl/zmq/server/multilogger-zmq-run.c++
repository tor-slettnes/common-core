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

        auto rpc_server = multilogger::zmq::Server::create_shared(
            api_provider,
            bind_address);

        //======================================================================
        // Initialize
        log_debug("Initializing MultiLogger ZMQ message publisher");
        message_publisher->initialize();

        log_debug("Initializing MultiLogger ZMQ message writer");
        message_writer->initialize();

        log_debug("Initializing MultiLogger ZMQ submission subscriber");
        submission_subscriber->initialize();

        log_debug("Initializing MultiLogger ZMQ submission handler");
        submission_handler->initialize();

        log_debug("Initializing MultiLogger ZMQ RPC server");
        rpc_server->initialize();

        //======================================================================
        // Run


        rpc_server->start();
        logf_notice("Multilogger ZMQ services are ready");
        core::platform::signal_shutdown.wait();

        logf_notice("Stopping MultiLogger ZMQ RPC server");
        rpc_server->stop();

        //======================================================================
        // Deinitialize

        logf_debug("Deinitializing MultiLogger ZMQ server");
        rpc_server->deinitialize();

        logf_debug("Deinitializing MultiLogger ZMQ submission handler");
        submission_handler->deinitialize();

        logf_debug("Deinitializing MultiLogger ZMQ submission subscriber");
        submission_subscriber->deinitialize();

        logf_debug("Deinitializing MultiLogger ZMQ message writer");
        message_writer->deinitialize();

        logf_debug("Deinitializing MultiLogger ZMQ message publisher");
        message_publisher->deinitialize();
    }

}  // namespace multilogger::zmq
