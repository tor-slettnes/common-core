/// -*- c++ -*-
//==============================================================================
/// @file kafka-callbacks.h++
/// @brief Miscellaneous callbacks to handle Kafka events
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/valuemap.h++"
#include "logging/logging.h++"
#include "status/error.h++"
#include <librdkafka/rdkafkacpp.h>

namespace core::kafka
{
    //--------------------------------------------------------------------------
    // LogCapture

    class LogCapture : public RdKafka::EventCb
    {
        using This = LogCapture;
        using LevelMap = core::types::ValueMap<RdKafka::Event::Severity, core::status::Level>;

    protected:
        void event_cb(RdKafka::Event &event) override;

    private:
        static const LevelMap level_map;
    };


    //--------------------------------------------------------------------------
    // DeliveryReportCapture

    class DeliveryReportCapture : public RdKafka::DeliveryReportCb
    {
        using This = DeliveryReportCapture;

    public:
        using Callback = std::function<void(const core::status::Error::ptr &error)>;

    public:
        void dr_cb(RdKafka::Message &message) override;
    };
}  // namespace core::kafka
