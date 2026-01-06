// -*- c++ -*-
//==============================================================================
/// @file options.h++
/// @brief Options parser for relay ZMQ client tool
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "common-options.h++"
#include "relay-zmq-subscriber.h++"
#include "relay-zmq-publisher.h++"

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
        std::shared_ptr<relay::zmq::Subscriber> subscriber_;
        std::shared_ptr<relay::zmq::Publisher> publisher_;
    };
}  // namespace relay
