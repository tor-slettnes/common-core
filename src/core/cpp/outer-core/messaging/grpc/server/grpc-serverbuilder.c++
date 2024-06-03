// -*- c++ -*-
//==============================================================================
/// @file grpc-serverbuilder.c++
/// @brief gRPC server builder with endpoint lookup
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-serverbuilder.h++"
#include "grpc-serverinterceptors.h++"

#include <grpc++/ext/proto_server_reflection_plugin.h>

namespace core::grpc
{
    ServerBuilder::ServerBuilder(
        const std::string &listen_address,
        bool enable_reflection,
        const std::shared_ptr<::grpc::ServerCredentials> &credentials)
        : ::grpc::ServerBuilder(),
          credentials_(credentials),
          max_request_size_(0),
          max_reply_size_(0)
    {
        if (enable_reflection)
        {
            ::grpc::reflection::InitProtoReflectionServerBuilderPlugin();
        }

        if (!listen_address.empty())
        {
            this->add_listener(listen_address);
        }
    }

    const std::vector<std::string> &ServerBuilder::listener_ports() const
    {
        return this->listeners_;
    }

    void ServerBuilder::add_handler_settings(std::shared_ptr<RequestHandlerBase> handler,
                                             bool add_listener)
    {
        if (add_listener)
        {
            this->add_listener(handler->address_setting());
        }
        this->adjust_max(handler->max_request_size(), &this->max_request_size_);
        this->adjust_max(handler->max_reply_size(), &this->max_reply_size_);
    }

    void ServerBuilder::add_listener(const std::string &address)
    {
        this->AddListeningPort(address, this->credentials_);
        this->listeners_.push_back(address);
    }

    void ServerBuilder::add_interceptors()
    {
        std::vector<std::unique_ptr<ServerInterceptorFactoryInterface>> creators;
        creators.push_back(LoggingInterceptorFactory::create_unique());
        // creators.push_back(EHInterceptorFactory::create_unique());
        this->experimental().SetInterceptorCreators(std::move(creators));
    }

    void ServerBuilder::adjust_max(uint candidate, uint *target)
    {
        if (candidate > *target)
        {
            *target = candidate;
        }
    }

    std::unique_ptr<::grpc::Server> ServerBuilder::BuildAndStart()
    {
        this->add_interceptors();

        if (this->max_request_size_)
        {
            this->SetMaxReceiveMessageSize(this->max_request_size_);
        }
        if (this->max_reply_size_)
        {
            this->SetMaxSendMessageSize(this->max_reply_size_);
        }
        return ::grpc::ServerBuilder::BuildAndStart();
    }

}  // namespace core::grpc
