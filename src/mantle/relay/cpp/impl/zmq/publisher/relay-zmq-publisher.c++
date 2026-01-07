// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-publisher.c++
/// @brief Publish relay signals over ZeroMQ
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-publisher.h++"
#include "relay-types.h++"
#include "parsers/json/writer.h++"

namespace relay::zmq
{
    Publisher::Publisher(const std::string &bind_address,
                         const std::string &channel_name,
                         Role role)
        : core::zmq::Publisher(bind_address, channel_name, role)
    {
    }

    void Publisher::initialize()
    {
        core::zmq::Publisher::initialize();
        relay::Publisher::initialize();
    }

    void Publisher::deinitialize()
    {
        relay::Publisher::deinitialize();
        core::zmq::Publisher::deinitialize();
    }

    void Publisher::start_writer()
    {
        this->signal_writer = SignalWriter::create_shared(this->shared_from_this());
        this->signal_writer->initialize();
        relay::Publisher::start_writer();
    }

    void Publisher::stop_writer()
    {
        relay::Publisher::stop_writer();
        if (this->signal_writer)
        {
            this->signal_writer->deinitialize();
            this->signal_writer.reset();
        }
    }

    bool Publisher::write(const std::string &topic,
                          const core::types::Value &payload)
    {
        core::zmq::Publisher::publish(
            core::types::ByteVector::from_string(topic),
            core::json::fast_writer.encoded(payload));
        return true;
    }

}  // namespace relay::zmq
