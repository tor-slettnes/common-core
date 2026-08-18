/// -*- c++ -*-
//==============================================================================
/// @file kafka-consumer-avro.c++
/// @brief Handle Avro payloads received over Kakfa
/// @author Tor Slettnes
//==============================================================================

#include "kafka-consumer-avro.h++"

namespace cc::kafka
{
    AvroConsumer::AvroConsumer(const std::string &profile_name,
                               const core::types::KeyValueMap settings,
                               const std::shared_ptr<sr::SchemaWrapper> &schema_wrapper)
        : Super(profile_name, settings),
          schema_wrapper(schema_wrapper)
    {
    }

    void AvroConsumer::register_schema_id(
        SchemaID schema_id,
        const std::shared_ptr<cc::avro::SchemaWrapper> &schema)
    {
        this->schemas_by_id.insert_or_assign(schema_id, schema);
    }

    void AvroConsumer::register_topic(
        const TopicName &topic,
        const std::shared_ptr<cc::avro::SchemaWrapper> &schema)
    {
        this->schemas_by_topic.insert_or_assign(topic, schema);
    }

    void AvroConsumer::handle_message(
        const core::dt::TimePoint &tp,
        const std::string &topic,
        const std::string &key,
        const HeaderMap &header,
        const core::types::ByteVector &payload)
    {
        if (this->schema_wrapper)
        {
            if (auto schema_id = this->schema_wrapper->extract_schema_id(payload))
            {
                if (auto schema = this->get_schema_by_id(schema_id.value()))
                {
                }
            }
        }
    }

    std::shared_ptr<cc::avro::SchemaWrapper> AvroConsumer::get_schema_by_id(
        SchemaID schema_id) const
    {
        return {};
    }

    std::shared_ptr<cc::avro::SchemaWrapper> AvroConsumer::get_schema_by_topic(
        const std::string &topic) const
    {
        return {};
    }
}  // namespace cc::kafka
