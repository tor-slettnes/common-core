/// -*- c++ -*-
//==============================================================================
/// @file zmq-publisher.h++
/// @brief Common functionality wrappers for ZeroMQ publishers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "zmq-publisher.h++"
#include "zmq-filter.h++"

namespace cc::zmq
{
    Publisher::Publisher(const std::string &class_name,
                         const std::string &channel_name,
                         const std::string &bind_address)
        : Super(class_name, channel_name, ::zmq::socket_type::pub, bind_address)
    {
    }

    std::vector<std::string> Publisher::settings_path() const
    {
        return {MESSAGE_GROUP, "publisher"};
    }

    void Publisher::publish(::zmq::message_t &&msg)
    {
        this->send(std::move(msg), ::zmq::send_flags::dontwait);
    }

    void Publisher::publish(const types::ByteArray &bytes)
    {
        this->send(bytes, ::zmq::send_flags::dontwait);
    }

    void Publisher::publish_with_topic(const std::string &topic,
                                       const types::ByteArray &bytes)
    {
        if (auto filter = Filter::create_from_topic(topic))
        {
            this->send(*filter,
                       ::zmq::send_flags::dontwait | ::zmq::send_flags::sndmore);
        }
        this->send(bytes);
    }

}  // namespace cc::zmq
