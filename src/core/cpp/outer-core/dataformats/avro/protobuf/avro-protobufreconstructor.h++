/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.h++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-compoundvalue.h++"
#include "avro-schemabuilder.h++"
#include "schema-wrapper.h++"
#include <google/protobuf/message.h>

namespace cc::avro
{
    bool reconstruct_proto_from_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata = nullptr);

    bool reconstruct_proto_from_tagged_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata = nullptr);

    bool reconstruct_proto_from_raw_payload(
        const core::types::ByteVector &payload,
        google::protobuf::Message *msg,
        google::protobuf::Message *metadata = nullptr);

    bool reconstruct_proto_from_avro_value(
        const CompoundValue &avro_value,
        google::protobuf::Message *msg);


}  // namespace cc::avro
