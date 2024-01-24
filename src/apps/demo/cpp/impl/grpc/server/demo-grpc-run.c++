// -*- c++ -*-
//==============================================================================
/// @file demo-grpc-server.c++
/// @brief Demo gRPC server
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "demo-grpc-run.h++"
#include "demo-grpc-requesthandler.h++"
#include "application/init.h++"
#include "chrono/date-time.h++"

namespace cc::demo::grpc
{
    void run_grpc_service(
        std::shared_ptr<cc::demo::API> api_provider)
    {
        constexpr auto SIGNAL_HANDLE = "gRPC Server";

        ::grpc::ServerBuilder builder;

        log_debug("Creating Demo gRPC request handler");
        auto request_handler = RequestHandler::create_shared(api_provider);
        request_handler->addToBuilder(&builder);

        log_debug("Starting Demo gRPC server");
        std::unique_ptr<::grpc::Server> server = builder.BuildAndStart();

        cc::application::signal_shutdown.connect(
            SIGNAL_HANDLE,
            [&](int signal) {
                server->Shutdown(dt::Clock::now() +
                                 std::chrono::seconds(5));
            });

        log_info("Demo gRPC server is ready on ", request_handler->serviceAddress);
        server->Wait();
        log_info("Demo gRPC server is shutting down");

        cc::application::signal_shutdown.disconnect(SIGNAL_HANDLE);
    }
}  // namespace cc::demo::grpc
