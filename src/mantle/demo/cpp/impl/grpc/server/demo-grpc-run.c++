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
        log_info("Creating gRPC server builder");
        cc::grpc::ServerBuilder builder(listen_address);

        log_info("Creating Demo gRPC request handler");
        auto request_handler = RequestHandler::create_shared(api_provider);
        builder.add_service(request_handler, listen_address.empty());

        log_info("Starting Demo gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        std::string signal_handle = cc::platform::signal_shutdown.connect(
            [&]() {
                log_info("Requesting gRPC server shutdown with a 5s timeout");
                server->Shutdown(cc::dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_notice("Demo gRPC server is ready on ", cc::str::join(builder.listener_ports()));
        server->Wait();
        log_notice("Demo gRPC server is shutting down");

        cc::platform::signal_shutdown.disconnect(signal_handle);
        server.reset();
    }
}  // namespace demo::grpc
