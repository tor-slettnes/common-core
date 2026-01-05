// -*- c++ -*-
//==============================================================================
/// @file pubsub-zmq-proxy.c++
/// @brief Pub/Sub Proxy daemon - ZMQ services
/// @author Tor Slettnes
//==============================================================================

#include "pubsub-zmq-proxy.h++"

namespace pubsub::zmq
{
    Proxy::Proxy(const std::string &proxy_subscriber_address,
                 const std::string &proxy_publisher_address)
        : xsub(proxy_subscriber_address, "forwarder", XSUB_CHANNEL, ZMQ_XSUB, Role::HOST),
          xpub(proxy_publisher_address, "forwarder", XPUB_CHANNEL, ZMQ_XPUB, Role::HOST)
    {
    }

    void Proxy::initialize()
    {
        this->xsub.initialize();
        this->xpub.initialize();
    }

    void Proxy::deinitialize()
    {
        this->xpub.deinitialize();
        this->xsub.deinitialize();
    }

    void Proxy::run()
    {
        zmq_proxy(this->xsub.socket(),
                  this->xpub.socket(),
                  nullptr);
    }
}  // namespace pubsub::zmq

