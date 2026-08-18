/// -*- c++ -*-
//==============================================================================
/// @file kafka-producer-avro.h++
/// @brief Send Avro payloads over Kafka
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "kafka-producer.h++"
#include "schema-wrapper.h++"
#include "avro-basevalue.h++"

namespace cc::kafka
{
    class AvroProducer : public Producer
    {
        using This = AvroProducer;
        using Super = Producer;

    protected:
        AvroProducer(const std::string &profile_name,
                     const core::types::KeyValueMap settings = {},
                     const std::shared_ptr<sr::SchemaWrapper> &schema_wrapper = {});

    public:
        using Super::produce;

        void produce(
            const std::string &topic,
            const avro::BaseValue &avro_wrapper,
            const std::optional<core::dt::TimePoint> &timepoint = {},
            const std::optional<std::string_view> &key = {},
            const HeaderMap &headers = {},
            const DeliveryReportCapture::CallbackData::ptr &cb_data = {});

        void produce(
            const std::string &topic,
            const avro::BaseValue &avro_wrapper,
            const sr::SchemaID schema_id,
            const std::optional<core::dt::TimePoint> &timepoint = {},
            const std::optional<std::string_view> &key = {},
            const HeaderMap &headers = {},
            const DeliveryReportCapture::CallbackData::ptr &cb_data = {});

    private:
        std::shared_ptr<sr::SchemaWrapper> schema_wrapper;

    };
}  // namespace cc::kafka
