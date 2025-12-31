// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-run.c++
/// @brief Demo gRPC server
/// @author Tor Slettnes
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
        log_info("Creating gRPC server builder");
        core::grpc::ServerBuilder builder(listen_address);

        log_info("Creating Demo gRPC request handler");
        auto request_handler = RequestHandler::create_shared(api_provider);
        builder.add_service(request_handler, listen_address.empty());

        log_info("Starting Demo gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        std::string signal_handle = core::platform::signal_shutdown.connect(
            [&]() {
                log_info("Requesting gRPC server shutdown with a 5s timeout");
                server->Shutdown(core::dt::Clock::now() +
                                 std::chrono::seconds(5));
                log_info("Requested gRPC server shutdown with a 5s timeout");
            });

        log_notice("Demo gRPC server is ready on ", core::str::join(builder.listener_ports()));
        server->Wait();
        log_notice("Demo gRPC server is shutting down");

        core::platform::signal_shutdown.disconnect(signal_handle);
        server.reset();
    }
}  // namespace demo::grpc
