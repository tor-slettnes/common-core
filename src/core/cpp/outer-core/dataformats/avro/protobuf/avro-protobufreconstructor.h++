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
        static bool reconstruct(
            const BaseValue& avro_value,
            ::google::protobuf::Message* msg);

    private:
        static bool reconstruct_record(
            const avro_value_t& avro_value,
            ::google::protobuf::Message* msg);

        static bool reconstruct_variant(
            const avro_value_t& avro_value,
            cc::protobuf::variant::Value* proto);

        static bool reconstruct_variant_list(
            const avro_value_t& avro_value,
            cc::protobuf::variant::ValueList* proto);

        static bool reconstruct_variant_map(
            const avro_value_t& avro_value,
            cc::protobuf::variant::KeyValueMap* proto);

        static bool reconstruct_wellknown(
            const avro_value_t& avro_value,
            ::google::protobuf::Message* msg);

        static bool reconstruct_custom(
            const avro_value_t& avro_value,
            ::google::protobuf::Message* msg);

        static bool reconstruct_field(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool reconstruct_single_field(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool reconstruct_repeated_field(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool reconstruct_repeated_element(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool reconstruct_mapped_field(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static std::optional<int> reconstruct_enum_value(
            const avro_value_t& avro_value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);
    };
}  // namespace cc::avro
