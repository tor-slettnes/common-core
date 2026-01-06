// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-publisher.c++
/// @brief Publish demo signals over ZeroMQ
/// @author Tor Slettnes
//==============================================================================

#include "demo-zmq-publisher.h++"
#include "demo-signals.h++"

namespace demo::zmq
{
    Publisher::Publisher(const std::string &bind_address,
                         const std::string &channel_name)
        : Super(bind_address, channel_name)
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
}  // namespace demo::zmq
