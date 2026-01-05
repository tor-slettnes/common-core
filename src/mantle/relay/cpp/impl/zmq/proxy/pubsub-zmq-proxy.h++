// -*- c++ -*-
//==============================================================================
/// @file pubsub-zmq-proxy.h++
/// @brief Pub/Sub Proxy daemon - ZMQ services
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "types/streamable.h++"
#include "types/create-shared.h++"

namespace pubsub::zmq
{
    constexpr auto XSUB_CHANNEL = "Subscriber Proxy";
    constexpr auto XPUB_CHANNEL = "Publisher Proxy";

    class Proxy: public core::types::Streamable,
                 public core::types::enable_create_shared<Proxy>
    {
        using This = Proxy;

    protected:
        Proxy(const std::string &proxy_subscriber_address,
              const std::string &proxy_publisher_address);

        void initialize();
        void deinitialize();
        void run();

    private:
        core::zmq::Endpoint xsub;
        core::zmq::Endpoint xpub;
    };
}
