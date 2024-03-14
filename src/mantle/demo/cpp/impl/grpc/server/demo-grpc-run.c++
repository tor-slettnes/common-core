// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-server.c++
/// @brief Demo gRPC server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-run.h++"
#include "demo-grpc-requesthandler.h++"
#include "grpc-serverbuilder.h++"
#include "platform/init.h++"
#include "chrono/date-time.h++"
#include "string/misc.h++"

namespace demo::grpc
{
    void run_grpc_service(
        std::shared_ptr<demo::API> api_provider,
        const std::string &listen_address)
    {
        constexpr auto SIGNAL_HANDLE = "gRPC Server";

        log_info("Creating gRPC server builder");
        core::grpc::ServerBuilder builder(listen_address);

        log_info("Creating Demo gRPC request handler");
        auto request_handler = RequestHandler::create_shared(api_provider);
        builder.add_service(request_handler, listen_address.empty());

        log_info("Starting Demo gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        core::platform::signal_shutdown.connect(
            SIGNAL_HANDLE,
            [&]() {
                log_info("Requesting gRPC server shutdown with a 5s timeout");
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_notice("Demo gRPC server is ready on ", core::str::join(builder.listener_ports()));
        server->Wait();
        log_notice("Demo gRPC server is shutting down");

        core::platform::signal_shutdown.disconnect(SIGNAL_HANDLE);
        server.reset();
    }
}  // namespace demo::grpc
