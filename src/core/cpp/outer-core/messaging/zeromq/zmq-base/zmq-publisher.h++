/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.h++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "types/bytevector.h++"

#include <thread>
#include <memory>

namespace core::zmq
{
    class Publisher : public Endpoint
    {
        using This = Publisher;
        using Super = Endpoint;

    public:
        using ptr = std::shared_ptr<This>;

    public:
        Publisher(const std::string &address,
                  const std::string &channel_name,
                  Role role = Role::HOST);

    public:
        void publish(const std::optional<types::ByteVector> &header,
                     const types::ByteVector &bytes);

    };

}  // namespace core::zmq
