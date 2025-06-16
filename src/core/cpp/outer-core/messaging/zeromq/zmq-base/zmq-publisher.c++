/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.c++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-publisher.h++"
#include "zmq-filter.h++"
#include "logging/logging.h++"

namespace core::zmq
{
    Publisher::Publisher(const std::string &bind_address,
                         const std::string &channel_name)
        : Super(bind_address, "publisher", channel_name, ZMQ_PUB)
    {
    }

    void Publisher::publish(const Filter &filter,
                            const types::ByteVector &bytes)
    {
        logf_trace("Publishing filter=%r, bytes=%r", filter, bytes);
        if (!filter.empty())
        {
            this->send(filter, ZMQ_SNDMORE);
        }
        this->send(bytes);
    }

}  // namespace core::zmq
