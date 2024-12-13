/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.h++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "avro-compoundvalue.h++"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace core::avro
{
    class ProtoBufValue : public CompoundValue
    {
        using This = ProtoBufValue;
        using Super = CompoundValue;

        using SchemaMap = std::unordered_map<const google::protobuf::Descriptor *,
                                             std::function<avro_schema_t()>>;

    private:
        static avro_schema_t schema(
            const google::protobuf::Descriptor *descriptor);

        static avro_schema_t custom_schema(
            const google::protobuf::Descriptor *descriptor);

    private:
        static const SchemaMap standard_schema_map;
    };
} // namespace core::avro
