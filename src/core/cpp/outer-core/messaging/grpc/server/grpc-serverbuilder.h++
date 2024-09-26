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

namespace core::grpc
{
    class ServerBuilder : public ::grpc::ServerBuilder
    {
    public:
        ServerBuilder(
            const std::string &listen_address = {},
            const std::shared_ptr<::grpc::ServerCredentials> &credentials =
                ::grpc::InsecureServerCredentials(),
            bool dnssd_advertise = true,
            bool enable_reflection = true);

        std::string listener_port() const;
        std::vector<std::string> listener_ports() const;
        std::unique_ptr<::grpc::Server> BuildAndStart() override;

        template <class HandlerT>
        void add_service(std::shared_ptr<HandlerT> handler,
                         bool add_listener = true)
        {
            this->RegisterService(handler.get());
            this->add_handler_settings(handler, add_listener);
            this->handlers_.push_back(handler);  // Keep from being destroyed
        }

    private:
        void add_handler_settings(std::shared_ptr<RequestHandlerBase> handler,
                                  bool add_listener);
        void add_listener(const std::string &address);
        void add_interceptors();
        void adjust_max(uint candidate, uint *target);

    private:
        std::vector<std::shared_ptr<RequestHandlerBase>> handlers_;
        std::string listen_address_;
        std::shared_ptr<::grpc::ServerCredentials> credentials_;
        uint max_request_size_;
        uint max_reply_size_;
        bool dnssd_advertise_;
        std::set<std::string> listeners_;
    };

}  // namespace core::grpc
