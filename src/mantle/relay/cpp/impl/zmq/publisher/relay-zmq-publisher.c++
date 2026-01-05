// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-publisher.c++
/// @brief Publish relay signals over ZeroMQ
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-publisher.h++"
#include "relay-common.h++"

namespace relay::zmq
{
    Publisher::Publisher(const std::string &bind_address,
                         const std::string &channel_name,
                         Role role)
        : Super(bind_address, channel_name, role)
    {
    }

    void Publisher::initialize()
    {
        Super::initialize();
        this->signal_writer = SignalWriter::create_shared(this->shared_from_this());
        this->signal_writer->initialize();
    }

    void Publisher::deinitialize()
    {
        if (this->signal_writer)
        {
            this->signal_writer->deinitialize();
            this->signal_writer.reset();
        }
        Super::deinitialize();
    }
}  // namespace relay::zmq
