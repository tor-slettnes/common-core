/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.h++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-protobufschema.h++"
#include "avro-basevalue.h++"
#include "types/shared-ptr-map.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace cc::avro
{
    class ProtoBufReconstructor
    {
        using This = ProtoBufReconstructor;

        using SchemaMap = core::types::ValueMap<
            ::google::protobuf::Descriptor*,
            std::shared_ptr<ProtoBufSchema>>;

    public:
        bool reconstruct(
            const BaseValue& avro_value,
            ::google::protobuf::Message* msg) const;

    private:
        static bool reconstruct_variant(
            const BaseValue& avro_value,
            cc::protobuf::variant::Value* proto);

        static bool reconstruct_variant_list(
            const BaseValue& avro_value,
            cc::protobuf::variant::ValueList* proto);

        static bool reconstruct_variant_map(
            const BaseValue& avro_value,
            cc::protobuf::variant::KeyValueMap* proto);

        static bool reconstruct_wellknown(
            const BaseValue& avro_value,
            ::google::protobuf::Message* msg);

        static bool reconstruct_custom(
            const BaseValue& avro_value,
            ::google::protobuf::Message* msg);
    };
}  // namespace cc::avro
