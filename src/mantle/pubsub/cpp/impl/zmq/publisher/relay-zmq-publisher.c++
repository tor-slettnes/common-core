// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-publisher.c++
/// @brief Publish relay signals over ZeroMQ
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-publisher.h++"
#include "relay-types.h++"
#include "parsers/json/writer.h++"

namespace cc::platform::pubsub::zmq
{
    Publisher::Publisher(const std::string& bind_address,
                         const std::string& channel_name,
                         Role role)
        : cc::zmq::Publisher(bind_address, channel_name, role)
    {
    }

    void Publisher::initialize()
    {
        cc::zmq::Publisher::initialize();
        pubsub::Publisher::initialize();
    }

    void Publisher::deinitialize()
    {
        pubsub::Publisher::deinitialize();
        cc::zmq::Publisher::deinitialize();
    }

    void Publisher::start_writer()
    {
        this->signal_writer = SignalWriter::create_shared(this->shared_from_this());
        this->signal_writer->initialize();
        pubsub::Publisher::start_writer();
    }

    void Publisher::stop_writer()
    {
        pubsub::Publisher::stop_writer();
        if (this->signal_writer)
        {
            this->signal_writer->deinitialize();
            this->signal_writer.reset();
        }
    }

    bool Publisher::write(const std::string& topic,
                          const core::types::Value& payload)
    {
        cc::zmq::Publisher::publish(
            core::types::ByteVector::from_string(topic),
            core::json::fast_writer.encoded(payload));
        return true;
    }

}  // namespace cc::platform::pubsub::zmq
