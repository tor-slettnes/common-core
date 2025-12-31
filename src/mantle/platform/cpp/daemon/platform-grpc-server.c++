// -*- c++ -*-
//==============================================================================
/// @file platform-grpc-server.c++
/// @brief Platform Server
/// @author Tor Slettnes
//==============================================================================

#include "platform-grpc-server.h++"
#include "sysconfig-grpc-requesthandler.h++"
#include "netconfig-grpc-requesthandler.h++"
#include "vfs-grpc-requesthandler.h++"
#include "upgrade-grpc-requesthandler.h++"
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

        builder.add_service(
            sysconfig::grpc::RequestHandler::create_shared(),  // handler
            listen_address.empty());                                     // add_listener

        builder.add_service(
            netconfig::grpc::RequestHandler::create_shared(),  // handler
            listen_address.empty());                                     // add_listener

        builder.add_service(
            vfs::grpc::RequestHandler::create_shared(),  // handler
            listen_address.empty());                               // add_listener

        builder.add_service(
            upgrade::grpc::RequestHandler::create_shared(),  // handler
            listen_address.empty());                                   // add_listener

        logf_debug("Starting gRPC Server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        log_debug("Adding shutdown handler for gRPC server");
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
