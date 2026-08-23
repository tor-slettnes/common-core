/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.h++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-basevalue.h++"
#include "avro-schemabuilder.h++"
#include "schema-wrapper.h++"
#include <google/protobuf/message.h>

namespace cc::avro
{
    bool reconstruct_proto_from_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg);

    bool reconstruct_proto_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg);

    bool reconstruct_proto_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg);

    bool reconstruct_proto_from_payload_and_schema(
        const core::types::ByteVector &payload,
        const SchemaWrapper &schema,
        google::protobuf::Message *msg);

    bool reconstruct_proto_from_avro_value(
        const BaseValue &avro_value,
        google::protobuf::Message *msg);

    bool reconstruct_proto_with_envelope(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *envelope);

    bool reconstruct_proto_with_envelope_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *envelope);

    bool reconstruct_proto_with_envelope_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *envelope);


}  // namespace cc::avro
