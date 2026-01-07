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
        using steady_clock = std::chrono::steady_clock;

    public:
        using ptr = std::shared_ptr<This>;

    public:
        Publisher(const std::string &address,
                  const std::string &channel_name,
                  Role role = Role::HOST,
                  steady_clock::duration warmup_delay = std::chrono::milliseconds(100));

    public:
        void initialize() override;

        void publish(const std::optional<types::ByteVector> &header,
                     const types::ByteVector &bytes);

    private:
        steady_clock::time_point warmup_deadline;
        steady_clock::duration warmup_delay;
    };

}  // namespace core::zmq
