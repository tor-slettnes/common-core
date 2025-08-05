/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer-avro.h++
/// @brief Send Avro payloads over Kafka
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "kafka-producer.h++"
#include "avro-basevalue.h++"

namespace core::kafka
{
    class AvroProducer : public Producer
    {
        using This = AvroProducer;
        using Super = Producer;

    protected:
        AvroProducer(const std::string &service_name,
                     const std::string &profile_name = {},
                     const core::types::KeyValueMap settings = {});

    public:
        using Super::produce;

        void produce(
            const std::string &topic,
            const avro::BaseValue &avro_wrapper,
            const std::optional<core::dt::TimePoint> &timepoint = {},
            const std::optional<std::string_view> &key = {},
            const HeaderMap &headers = {},
            const DeliveryReportCapture::CallbackData::ptr &cb_data = {});
    };
}  // namespace core::kafka
