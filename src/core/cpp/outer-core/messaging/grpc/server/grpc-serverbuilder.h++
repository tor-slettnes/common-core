// -*- c++ -*-
//==============================================================================
/// @file grpc-serverbuilder.h++
/// @brief gRPC server builder with endpoint lookup
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "grpc-base.h++"
#include "grpc-requesthandler.h++"
#include <grpc++/server_builder.h>

namespace cc::grpc
{
    class ServerBuilder : public ::grpc::ServerBuilder
    {
    public:
        ServerBuilder(
            const std::string &listen_address = {},
            bool enable_reflection = true,
            const std::shared_ptr<::grpc::ServerCredentials> &credentials =
                ::grpc::InsecureServerCredentials());

        const std::vector<std::string> &listener_ports() const;
        std::unique_ptr<::grpc::Server> BuildAndStart() override;

        template <class HandlerT>
        void add_service(std::shared_ptr<HandlerT> handler,
                         bool add_listener = true)
        {
            this->RegisterService(handler.get());
            this->add_handler_settings(handler, add_listener);
        }

    private:
        void add_handler_settings(std::shared_ptr<RequestHandlerBase> handler,
                                  bool add_listener);
        void add_listener(const std::string &address);
        void add_interceptors();
        void adjust_max(uint candidate, uint *target);

    private:
        std::shared_ptr<::grpc::ServerCredentials> credentials_;
        uint max_request_size_;
        uint max_reply_size_;
        std::vector<std::string> listeners_;
    };

}  // namespace cc::grpc
