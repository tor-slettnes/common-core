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

        message_publisher->initialize();
        message_writer->initialize();

        submission_handler->initialize();
        submission_subscriber->initialize();

        rpc_server->initialize();

        //======================================================================
        // Run

        log_debug("Adding ZMQ shutdown handler");
        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                log_info("ZMQ RPC server is stopping");
                rpc_server->stop();
                log_info("ZMQ RPC server has stopped");
            });


        logf_notice("Multilogger ZMQ services are ready");
        rpc_server->run();
        logf_notice("Multilogger ZMQ services are shutting down");

        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);

        //======================================================================
        // Deinitialize

        logf_debug("Deinitializing ZMQ RPC server");
        rpc_server->deinitialize();

        logf_debug("Deinitializing ZMQ submission subscriber");
        submission_subscriber->deinitialize();

        logf_debug("Deinitializing ZMQ submission handler");
        submission_handler->deinitialize();

        logf_debug("Deinitializing ZMQ submission writer");
        message_writer->deinitialize();

        logf_debug("Deinitializing ZMQ submission publisher");
        message_publisher->deinitialize();
    }

}  // namespace multilogger::zmq
