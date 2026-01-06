// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for relay tool
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "common-options.h++"
#include "relay-grpc-client.h++"
#include "relay-subscriber.h++"
#include "relay-publisher.h++"

namespace relay::grpc
{
    class Options : public relay::CommonOptions
    {
        using This = Options;
        using Super = relay::CommonOptions;

    public:
        Options();
        ~Options();

    protected:
        std::shared_ptr<relay::Subscriber> subscriber() override;
        std::shared_ptr<relay::Publisher> publisher() override;

    private:
        std::shared_ptr<relay::grpc::Client> client();

    private:
        std::shared_ptr<relay::grpc::Client> client_;
    };
}  // namespace relay
