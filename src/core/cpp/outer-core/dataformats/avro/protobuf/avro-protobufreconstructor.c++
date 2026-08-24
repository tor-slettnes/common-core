/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.c++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufreconstructor.h++"
#include "avro-protobufschema.h++"
#include "avro-compoundvalue.h++"
#include "schema-wrapper.h++"
#include "protobuf-message-encoder.h++"
#include "logging/logging.h++"

namespace cc::avro
{
    bool reconstruct_proto_from_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg)
    {
        if (sr::extract_schema_id(payload))
        {
            return reconstruct_proto_from_tagged_payload(payload, msg);
        }
        else
        {
            return reconstruct_proto_from_raw_payload(payload, msg);
        }
    }

    bool reconstruct_proto_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg)
    {
        if (auto serialized = sr::extract_payload(payload))
        {
            return reconstruct_proto_from_raw_payload(*serialized, msg);
        }
        else
        {
            return false;
        }
    }

    bool reconstruct_proto_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg)
    {
        return reconstruct_proto_from_payload_and_schema(
            payload,
            ProtoBufSchema::from_proto(msg->GetDescriptor()),
            msg);
    }

    bool reconstruct_proto_from_payload_and_schema(
        const core::types::ByteVector &payload,
        const SchemaWrapper &schema,
        google::protobuf::Message *msg)
    {
        CompoundValue avro_value(schema);
        avro_value.set_from_serialized(payload);
        return reconstruct_proto_from_avro_value(avro_value, msg);
    }

    bool reconstruct_proto_from_avro_value(
        const BaseValue &avro_value,
        google::protobuf::Message *msg)
    {
        return cc::protobuf::encode_to_message(
            avro_value.as_value(),
            msg);
    }

    bool reconstruct_proto_with_metadata(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        if (sr::extract_schema_id(payload))
        {
            return reconstruct_proto_with_metadata_from_tagged_payload(
                payload,
                msg,
                metadata);
        }
        else
        {
            return reconstruct_proto_with_metadata_from_raw_payload(
                payload,
                msg,
                metadata);
        }
    }

    bool reconstruct_proto_with_metadata_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        if (auto serialized = sr::extract_payload(payload))
        {
            return reconstruct_proto_with_metadata_from_raw_payload(
                *serialized,
                msg,
                metadata);
        }
        else
        {
            return false;
        }
    }

    bool reconstruct_proto_with_metadata_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata)
    {
        auto schema = ProtoBufSchema::from_proto(
            msg->GetDescriptor(),
            metadata->GetDescriptor());

        CompoundValue wrapper(schema);
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
}  // namespace cc::avro
