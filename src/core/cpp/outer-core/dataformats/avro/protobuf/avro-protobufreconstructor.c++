/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.c++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufreconstructor.h++"
#include "avro-protobufschema.h++"
#include "schema-wrapper.h++"
#include "protobuf-message-encoder.h++"
#include "logging/logging.h++"

namespace cc::avro
{
    bool reconstruct_proto_from_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        if (auto schema_id = sr::extract_schema_id(payload))
        {
            return reconstruct_proto_from_tagged_payload(payload, msg, metadata);
        }
        else
        {
            return reconstruct_proto_from_raw_payload(payload, msg, metadata);
        }
    }

    bool reconstruct_proto_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        if (auto serialized = sr::extract_payload(payload))
        {
            return reconstruct_proto_from_raw_payload(*serialized, msg, metadata);
        }
        else
        {
            return false;
        }
    }

    bool reconstruct_proto_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        const SchemaWrapper &schema = ProtoBufSchema::from_proto(
            msg->GetDescriptor(),
            metadata ? metadata->GetDescriptor() : nullptr);

        CompoundValue wrapper(schema.as_avro_schema(), false);
        wrapper.set_from_serialized(payload);

        bool success = true;
        if (metadata)
        {
            success &= reconstruct_proto_from_avro_value(
                wrapper.get_field_by_index(0, METADATA_FIELD),
                metadata);
        }

        if (success && msg)
        {
            success &= reconstruct_proto_from_avro_value(
                wrapper,
                msg);
        }

        return success;
    }

    bool reconstruct_proto_from_avro_value(
        const CompoundValue &avro_value,
        google::protobuf::Message *msg)
    {
        return cc::protobuf::encode_to_message(
            avro_value.as_value(),
            msg);
    }
}  // namespace cc::avro
