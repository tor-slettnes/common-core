/// -*- c++ -*-
//==============================================================================
/// @file kafka-consumer-avro.h++
/// @brief Handle Avro payloads received over Kakfa
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "kafka-consumer.h++"
#include "avro-schemabuilder.h++"
#include "avro-basevalue.h++"
#include "schema-wrapper.h++"
#include "types/valuemap.h++"

namespace cc::kafka
{
    using sr::SchemaID;

    class AvroConsumer : public Consumer
    {
        using This = AvroConsumer;
        using Super = Consumer;

        using IdToSchemaMap = core::types::ValueMap<
            SchemaID,
            std::shared_ptr<cc::avro::SchemaWrapper>>;

        using TopicToSchemaMap = core::types::ValueMap<
            TopicName,
            std::shared_ptr<cc::avro::SchemaWrapper>>;

        using AvroMessageHandler = std::function<void(
            const core::dt::TimePoint &tp,
            const std::string &key,
            const core::types::KeyValueMap &header,
            const avro::BaseValue &avro_value)>;

    protected:
        AvroConsumer(const std::string &profile_name,
                     const core::types::KeyValueMap settings = {},
                     const std::shared_ptr<sr::SchemaWrapper> &schema_wrapper = {});

    public:
        using Super::handle_message;

        void register_schema_id(
            SchemaID schema_id,
            const std::shared_ptr<cc::avro::SchemaWrapper> &schema);

        void register_topic(
            const TopicName &topic,
            const std::shared_ptr<cc::avro::SchemaWrapper> &schema);

        void handle_message(
            const core::dt::TimePoint &tp,
            const std::string &topic,
            const std::string &key,
            const HeaderMap &header,
            const core::types::ByteVector &payload) override;

    protected:
        virtual std::shared_ptr<cc::avro::SchemaWrapper> get_schema_by_id(
            SchemaID schema_id) const;

        virtual std::shared_ptr<cc::avro::SchemaWrapper> get_schema_by_topic(
            const std::string &topic) const;

    protected:
        std::shared_ptr<sr::SchemaWrapper> schema_wrapper;
        IdToSchemaMap schemas_by_id;
        TopicToSchemaMap schemas_by_topic;
    };
}  // namespace cc::kafka
