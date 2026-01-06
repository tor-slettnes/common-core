// -*- c++ -*-
//==============================================================================
/// @file options.c++
/// @brief Options parser for relay ZMQ client tool
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
        if (auto subscriber = this->subscriber_)
        {
            subscriber->deinitialize();
        }

        if (auto publisher = this->publisher_)
        {
            publisher->deinitialize();
        }
    }

    std::shared_ptr<relay::Subscriber> Options::subscriber()
    {
        if (!this->subscriber_)
        {
            this->subscriber_ = relay::zmq::Subscriber::create_shared(this->host);
            this->subscriber_->initialize();
        }
        return this->subscriber_;
    }

    std::shared_ptr<relay::Publisher> Options::publisher()
    {
        if (!this->publisher_)
        {
            this->publisher_ = relay::zmq::Publisher::create_shared(this->host);
            this->publisher_->initialize();
        }
        return this->publisher_;
    }
}  // namespace relay
