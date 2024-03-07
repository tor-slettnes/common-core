// -*- c++ -*-
//==============================================================================
/// @file grpc-serverbuilder.h++
/// @brief gRPC server builder with endpoint lookup
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "grpc-serverbuilder.h++"

namespace core::grpc
{
    ServerBuilder::ServerBuilder(
        const std::string &listen_address,
        const std::shared_ptr<::grpc::ServerCredentials> &credentials)
        : ::grpc::ServerBuilder(),
          credentials_(credentials),
          max_request_size_(0),
          max_reply_size_(0)
    {
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

    void ServerBuilder::adjust_max(uint candidate, uint *target)
    {
        if (candidate > *target)
        {
            *target = candidate;
        }
    }

    std::unique_ptr<::grpc::Server> ServerBuilder::BuildAndStart()
    {
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
