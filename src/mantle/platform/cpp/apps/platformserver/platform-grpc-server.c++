// -*- c++ -*-
//==============================================================================
/// @file grpc-service.c++
/// @brief Platform Server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "platform-grpc-server.h++"
#include "system-grpc-requesthandler.h++"
#include "grpc-serverbuilder.h++"
#include "logging/logging.h++"

#include <memory>
#include <thread>

namespace platform
{
    void run_grpc_service(
        const std::string &listen_address)
    {
        log_debug("Creating gRPC server builder");
        core::grpc::ServerBuilder builder(listen_address);

        auto system_handler = platform::system::grpc::RequestHandler::create_shared();
        builder.add_service(system_handler,           // handler
                            listen_address.empty());  // add_listener

        logf_debug("Starting gRPC Server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        log_debug("Adding shutdown handler");
        std::string signal_handle = core::platform::signal_shutdown.connect(
            [&]() {
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_notice("gRPC server is ready on ",
                   core::str::join(builder.listener_ports()));
        server->Wait();

        log_notice("gRPC server is shutting down");

        core::platform::signal_shutdown.disconnect(signal_handle);
        server.reset();
    }
}  // namespace platform