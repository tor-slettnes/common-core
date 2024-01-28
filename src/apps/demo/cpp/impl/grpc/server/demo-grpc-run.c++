// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-server.c++
/// @brief Demo gRPC server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-run.h++"
#include "demo-grpc-requesthandler.h++"
#include "grpc-serverbuilder.h++"
#include "application/init.h++"
#include "chrono/date-time.h++"
#include "string/misc.h++"

namespace cc::demo::grpc
{
    void run_grpc_service(
        std::shared_ptr<cc::demo::API> api_provider,
        const std::string &listen_address)
    {
        constexpr auto SIGNAL_HANDLE = "gRPC Server";

        log_debug("Creating gRPC server builder");
        cc::grpc::ServerBuilder builder(listen_address);

        log_debug("Creating Demo gRPC request handler");
        auto request_handler = RequestHandler::create_shared(api_provider);
        builder.add_service(request_handler, listen_address.empty());

        log_debug("Starting Demo gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        cc::application::signal_shutdown.connect(
            SIGNAL_HANDLE,
            [&](int signal) {
                server->Shutdown(dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_info("Demo gRPC server is ready on ", str::join(builder.listener_ports()));
        server->Wait();
        log_info("Demo gRPC server is shutting down");

        cc::application::signal_shutdown.disconnect(SIGNAL_HANDLE);
    }
}  // namespace cc::demo::grpc
