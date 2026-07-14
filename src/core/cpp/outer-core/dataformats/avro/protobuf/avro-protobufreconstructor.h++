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
    class ProtoBufBuilder
    {
        using This = ProtoBufBuilder;

        using SchemaMap = core::types::ValueMap<
            ::google::protobuf::Descriptor *,
            std::shared_ptr<ProtoBufSchema>>;

    public:
        void reconstruct(
            const BaseValue &avro_value,
            ::google::protobuf::Message *msg) const;

    private:
        static void reconstruct_variant(
            const BaseValue &avro_value,
            cc::protobuf::variant::Value *value);

        static void reconstruct_variant_list(
            const BaseValue &avro_value,
            cc::protobuf::variant::ValueList *list);

        static void reconstruct_variant_map(
            const BaseValue &avro_value,
            cc::protobuf::variant::KeyValueMap *kvmap);

        static void reconstruct_wellknown(
            const BaseValue &avro_value,
            ::google::protobuf::Message *msg);

        static void reconstruct_custom(
            const BaseValue &avro_value,
            ::google::protobuf::Message *msg);


    };
}  // namespace cc::avro
