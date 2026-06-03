/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.c++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes
//==============================================================================

#include "zmq-publisher.h++"
#include "logging/logging.h++"

namespace cc::zmq
{
    Publisher::Publisher(const std::string &address,
                         const std::string &channel_name,
                         Role role,
                         steady_clock::duration warmup_delay)
        : Super(address, "publisher", channel_name, ZMQ_PUB, role),
          warmup_delay(warmup_delay)
    {
    }

    void Publisher::initialize()
    {
        Super::initialize();
        this->warmup_deadline = steady_clock::now() + this->warmup_delay;
    }

    void Publisher::publish(const std::optional<core::types::ByteVector> &header,
                            const core::types::ByteVector &bytes)
    {
        if (header)
        {
            if (steady_clock::now() < this->warmup_deadline)
            {
                logf_debug("%s: Holding for initial warmup", *this);
                std::this_thread::sleep_until(this->warmup_deadline);
            }
            logf_trace("%s: Publishing with header=%r, bytes=%r", *this, header, bytes);
            this->send(*header, ZMQ_SNDMORE);
        }
        else
        {
            logf_trace("%s: Publishing without header, bytes=%r", *this, bytes);
        }
        this->send(bytes);
    }

}  // namespace cc::zmq
