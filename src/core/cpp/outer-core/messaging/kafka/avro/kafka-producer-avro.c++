/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer-avro.c++
/// @brief Send Avro payloads over Kafka
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "kafka-producer-avro.h++"

namespace core::kafka
{
    AvroProducer::AvroProducer(const std::string &service_name)
        : Producer(service_name)
    {
    }

    void AvroProducer::produce(
            const std::string_view &topic,
            const avro::BaseValue &avro_wrapper,
            const std::optional<std::string_view> &key)
    {
        this->produce(
            topic,
            avro_wrapper.serialized(),
            key);
    }

}  // namespace core::kafka
