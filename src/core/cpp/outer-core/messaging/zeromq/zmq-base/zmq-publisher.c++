/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.c++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-publisher.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    Publisher::Publisher(const std::string &address,
                         const std::string &channel_name,
                         Role role)
        : Super(address, "publisher", channel_name, ZMQ_PUB, role)
    {
    }

    void Publisher::publish(const std::optional<types::ByteVector> &header,
                            const types::ByteVector &bytes)
    {
        if (header)
        {
            logf_trace("Publishing with header=%r, bytes=%r", header, bytes);
            this->send(*header, ZMQ_SNDMORE);
        }
        else
        {
            logf_trace("Publishing without header, bytes=%r", bytes);
        }
        this->send(bytes);
    }

}  // namespace core::zmq
