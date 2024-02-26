/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.h++
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
        : Super(bind_address,
                "publisher",
                channel_name,
                ::zmq::socket_type::pub)
    {
    }

    void Publisher::publish(const Filter &filter,
                            const types::ByteVector &bytes)
    {
        logf_trace("Publishing filter=%r, bytes=%r", filter, bytes);
        if (!filter.empty())
        {
            this->send(filter, ::zmq::send_flags::dontwait | ::zmq::send_flags::sndmore);
        }
        this->send(bytes);
    }

}  // namespace core::zmq
