/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer-avro.c++
/// @brief Send Avro payloads over Kafka
/// @author Tor Slettnes
//==============================================================================

#include "kafka-producer-avro.h++"
#include "status/exceptions.h++"

namespace cc::kafka
{
    AvroProducer::AvroProducer(const std::string &profile_name,
                               const core::types::KeyValueMap settings)
        : Super(profile_name, settings)
    {
    }

    void AvroProducer::produce(
        const std::string &topic,
        const avro::BaseValue &avro_wrapper,
        const std::optional<core::dt::TimePoint> &timepoint,
        const std::optional<std::string_view> &key,
        const HeaderMap &headers,
        const DeliveryReportCapture::CallbackData::ptr &cb_data)
    {
        this->produce(
            topic,
            avro_wrapper.serialized(),
            timepoint,
            key,
            headers,
            cb_data);
    }

    void AvroProducer::produce(
        const std::string &topic,
        const avro::BaseValue &avro_wrapper,
        const sr::SchemaID schema_id,
        const std::optional<core::dt::TimePoint> &timepoint,
        const std::optional<std::string_view> &key,
        const HeaderMap &headers,
        const DeliveryReportCapture::CallbackData::ptr &cb_data)
    {
        this->produce(
            topic,
            sr::wrap(avro_wrapper.serialized(), schema_id),
            timepoint,
            key,
            headers,
            cb_data);
    }
}  // namespace cc::kafka
