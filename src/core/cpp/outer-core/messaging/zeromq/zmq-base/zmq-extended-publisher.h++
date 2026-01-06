/// -*- c++ -*-
//==============================================================================
/// @file zmq-extended-publisher.h++
/// @brief ZMQ XPUB endpoint
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"

namespace core::zmq
{
    class ExtendedPublisher : public Endpoint
    {
        using This = ExtendedPublisher;
        using Super = Endpoint;

    public:
        using ptr = std::shared_ptr<This>;

    public:
        ExtendedPublisher(const std::string &address,
                          const std::string &channel_name,
                          Role role = Role::HOST);
    };

}  // namespace core::zmq
