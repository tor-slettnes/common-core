// -*- c++ -*-
//==============================================================================
/// @file relay-zmq-signalwriter.c++
/// @brief Forward local RELAY signals over ZMQ
/// @author Tor Slettnes
//==============================================================================

#include "relay-zmq-signalwriter.h++"
#include "relay-common.h++"

#include "platform/symbols.h++"
#include "parsers/json/writer.h++"

namespace relay::zmq
{
    void SignalWriter::initialize()
    {
        Super::initialize();

        // Connect local signals from `api/relay-common.h++` to this writer.
        using namespace std::placeholders;
        relay::signal_message.connect(
            TYPE_NAME_FULL(This),
            std::bind(&SignalWriter::write_message, this, _1, _2, _3));
    }

    void SignalWriter::deinitialize()
    {
        relay::signal_message.disconnect(TYPE_NAME_FULL(This));
        Super::deinitialize();
    }

    void SignalWriter::write_message(
        core::signal::MappingAction action,
        const std::string &topic,
        const core::types::Value &payload)
    {
        switch (action)
        {
        case core::signal::MAP_ADDITION:
        case core::signal::MAP_UPDATE:
            if (auto publisher = this->publisher())
            {
                if (publisher->initialized())
                {
                    publisher->publish(
                        topic,
                        core::json::fast_writer.encoded(payload));
                }
            }
            break;
        }
    }

}  // namespace relay::zmq
