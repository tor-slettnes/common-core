/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.h++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "zmq-endpoint.h++"
#include "zmq-filter.h++"
#include "types/bytevector.h++"

#include <thread>
#include <memory>

namespace core::zmq
{
    class Publisher : public Endpoint
    {
        using This = Publisher;
        using Super = Endpoint;

    protected:
        Publisher(const std::string &address,
                  const std::string &channel_name,
                  Role role = Role::HOST);

    public:
        void publish(const Filter &filter,
                     const types::ByteVector &bytes);
    };

}  // namespace core::zmq
