// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for relay tool
/// @author Tor Slettnes
//==============================================================================

#include "options.h++"

namespace relay::grpc
{
    Options::Options()
        : Super("gRPC")
    {
    }

    Options::~Options()
    {
        if (auto client = this->client_)
        {
            client->deinitialize();
        }
    }


    std::shared_ptr<relay::Subscriber> Options::subscriber()
    {
        return this->client();
    }

    std::shared_ptr<relay::Publisher> Options::publisher()
    {
        return this->client();
    }

    relay::grpc::Client::ptr Options::client()
    {
        if (!this->client_)
        {
            this->client_ = relay::grpc::Client::create_shared(this->host);
            this->client_->initialize();
        }
        return this->client_;
    }
}  // namespace relay
