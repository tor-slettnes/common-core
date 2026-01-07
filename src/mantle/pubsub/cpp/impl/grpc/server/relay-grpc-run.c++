// -*- c++ -*-
//==============================================================================
/// @file relay-grpc-run.c++
/// @brief Relay gRPC server
/// @author Tor Slettnes
//==============================================================================

#include "relay-grpc-run.h++"
#include "relay-grpc-requesthandler.h++"
#include "grpc-serverbuilder.h++"
#include "platform/init.h++"
#include "logging/logging.h++"
#include "chrono/date-time.h++"
#include "string/misc.h++"

namespace pubsub::grpc
{
    void run_grpc_service(
        const std::string &listen_address)
    {
        log_debug("Creating gRPC server builder");
        core::grpc::ServerBuilder builder(listen_address);

        log_debug("Creating Relay gRPC request handler");
        auto request_handler = RequestHandler::create_shared();
        builder.add_service(request_handler, listen_address.empty());

        log_debug("Starting Relay gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        std::string signal_handle = core::platform::signal_shutdown.connect(
            [&]() {
                log_info("Requesting gRPC server shutdown with a 5s timeout");
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
                log_info("Requested gRPC server shutdown with a 5s timeout");
            });

        log_notice("Relay gRPC server is ready on ", core::str::join(builder.listener_ports()));
        server->Wait();
        log_notice("Relay gRPC server is shutting down");

        core::platform::signal_shutdown.disconnect(signal_handle);
        server.reset();
    }
}  // namespace pubsub::grpc
