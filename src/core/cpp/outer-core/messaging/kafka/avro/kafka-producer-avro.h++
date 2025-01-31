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
                     const std::string &server_address = {});

    public:
        using Super::produce;

        void produce(
            const std::string_view &topic,
            const avro::BaseValue &avro_wrapper,
            const std::optional<std::string_view> &key = {});
    };
}  // namespace core::kafka
