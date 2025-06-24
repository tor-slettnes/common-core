// -*- c++ -*-
//==============================================================================
/// @file grpc-service.c++
/// @brief Launch MultiLogger gRPC services
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "multilogger-grpc-run.h++"
#include "multilogger-grpc-requesthandler.h++"
#include "grpc-serverbuilder.h++"
#include "platform/init.h++"
#include "logging/logging.h++"


namespace multilogger::grpc
{
    constexpr auto SHUTDOWN_SIGNAL_HANDLE = "multilogger-grpc-service";

    void run_service(
        std::shared_ptr<API> multilogger_provider,
        const std::string &listen_address)
    {
        log_debug("Creating gRPC server builder");
        core::grpc::ServerBuilder builder(listen_address);

        if (multilogger_provider)
        {
            log_debug("Creating gRPC request handler: multilogger::API");
            builder.add_service(
                multilogger::grpc::RequestHandler::create_shared(multilogger_provider),
                listen_address.empty());  // add_listener
        }

        log_debug("Starting gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        log_debug("Adding shutdown handler");
        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                log_info("Multilogger gRPC service is shutting down");
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
                log_info("Multilogger gRPC service is down");
            });

        log_notice("Multilogger gRPC server is ready on ", core::str::join(builder.listener_ports()));
        server->Wait();

        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);
        server.reset();
    }
}  // namespace multilogger::grpc
