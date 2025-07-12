/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer-avro.c++
/// @brief Send Avro payloads over Kafka
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-producer-avro.h++"

namespace core::kafka
{
    AvroProducer::AvroProducer(const std::string &service_name,
                               const std::string &profile_name,
                               const core::types::KeyValueMap settings)
        : Producer(service_name, profile_name, settings)
    {
    }

    void AvroProducer::produce(
        const std::string &topic,
        const avro::BaseValue &avro_wrapper,
        const std::optional<std::string_view> &key,
        const HeaderMap &headers,
        const DeliveryReportCapture::CallbackData::ptr &cb_data)
    {
        this->produce(
            topic,
            avro_wrapper.serialized(),
            key,
            headers,
            cb_data);
    }

}  // namespace core::kafka
