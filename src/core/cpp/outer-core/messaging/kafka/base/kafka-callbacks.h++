/// -*- c++ -*-
//==============================================================================
/// @file kafka-callbacks.h++
/// @brief Miscellaneous callbacks to handle Kafka events
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/valuemap.h++"
#include "logging/logging.h++"
#include <librdkafka/rdkafkacpp.h>

namespace core::kafka
{
    class LogCapture : public RdKafka::EventCb
    {
        using This = LogCapture;
        using LevelMap = core::types::ValueMap<RdKafka::Event::Severity, core::status::Level>;

    protected:
        void event_cb(RdKafka::Event &event) override;

    private:
        static const LevelMap level_map;
    };
}  // namespace core::kafka
