// -*- c++ -*-
//==============================================================================
/// @file grpc-service.c++
/// @brief Launch Logger gRPC services
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "grpc-service.h++"
#include "grpc-serverbuilder.h++"
#include "logger-grpc-requesthandler.h++"

constexpr auto SHUTDOWN_SIGNAL_HANDLE = "logger-grpc-service";

namespace logger::grpc
{
    void run_service(
        std::shared_ptr<API> logger_provider,
        const std::string &listen_address)
    {
        log_debug("Creating gRPC server builder");
        core::grpc::ServerBuilder(listen_address);

        if (logger_provider)
        {
            log_debug("Creating gRPC request handler: logger::API");
            auto logger_handler = logger::grpc::RequestHandler(logger_provider);
            builder.add_service(logger_handler,          // handler
                                listen_address.empty());  // add_listener
        }

        log_debug("Starting gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        log_debug("Adding shutdown handler");
        core::platform::signal_shutdown.connect(
            SHUTDOWN_SIGNAL_HANDLE,
            [&]() {
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_notice("gRPC server is ready on ", core::str::join(builder.listener_ports()));
        server->Wait();

        log_notice("Shutting down gRPC server");
        core::platform::signal_shutdown.disconnect(SHUTDOWN_SIGNAL_HANDLE);
        server.reset();
    }
}  // namespace logger::grpc
