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
    bool reconstruct_proto(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg)
    {
        sr::SchemaID schema_id = 0;
        core::types::ByteVector serialized_avro;
        if (sr::unwrap(payload, &schema_id, &serialized_avro))
        {
            logf_debug(
                "Reconstructing ProtoBuf message %r from "
                "serialized Avro payload; schema_id=%d, size=%d",
                msg->GetDescriptor()->full_name(),
                schema_id,
                serialized_avro.size());

            return reconstruct_proto(
                serialized_avro,
                ProtoBufSchema::from_proto(msg->GetDescriptor()),
                msg);
        }
        else
        {
            logf_notice(
                "Reconstructing ProtoBuf message %r from "
                "serialized Avro payload with no schema ID; size=%d",
                msg->GetDescriptor()->full_name(),
                payload.size());

            return reconstruct_proto(
                payload,
                ProtoBufSchema::from_proto(msg->GetDescriptor()),
                msg);
        }
    }

    bool reconstruct_proto(
        const core::types::ByteVector &payload,
        const SchemaWrapper &schema,
        google::protobuf::Message *msg)
    {
        return reconstruct_proto(
            CompoundValue(schema),
            msg);
    }

    bool reconstruct_proto(
        const BaseValue &avro_value,
        google::protobuf::Message *msg)
    {
        return cc::protobuf::encode_to_message(
            avro_value.as_value(),
            msg);
    }

}  // namespace cc::avro
