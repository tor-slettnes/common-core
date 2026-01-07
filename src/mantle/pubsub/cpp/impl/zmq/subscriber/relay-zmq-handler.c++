// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-handler.c++
/// @brief Relay - ZMQ message receiver
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-handler.h++"
#include "relay-types.h++"
#include "parsers/json/reader.h++"
#include "logging/logging.h++"
#include "platform/symbols.h++"

namespace pubsub::zmq
{
    Handler::Handler()
        : Super(TYPE_NAME_FULL(This))
    {
    }

    void Handler::handle(const core::zmq::MessageParts &parts)
    {
        if (parts.size() >= 2)
        {
            std::string topic = parts.at(0).as_string();
            std::string_view payload = parts.at(1).stringview();
            core::types::Value value = core::json::reader.decoded(payload);
            pubsub::signal_publication.emit(topic, value);
        }
        else
        {
            logf_notice("Received short ZMQ message with %d of 2 required parts: %s",
                        parts.size(),
                        parts);
        }
    }
}  // namespace pubsub::zmq
