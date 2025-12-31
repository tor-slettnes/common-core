/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.h++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "avro-compoundvalue.h++"

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

namespace avro
{
    class ProtoBufValue : public CompoundValue
    {
        using This = ProtoBufValue;
        using Super = CompoundValue;

    public:
        ProtoBufValue(const google::protobuf::Message &msg);

        ProtoBufValue &assign_from_message(
            const google::protobuf::Message &msg);

    private:
        static void assign_from_message(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg);

        static void assign_from_wellknown(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg);

        static void assign_from_custom(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg);

        static void assign_from_field(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg,
            const google::protobuf::FieldDescriptor *fd);

        static void assign_from_single_field(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg,
            const google::protobuf::FieldDescriptor *fd);

        static void assign_from_indexed_field(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg,
            const google::protobuf::FieldDescriptor *fd,
            int index);

        static void assign_from_repeated_field(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg,
            const google::protobuf::FieldDescriptor *fd);

        static void assign_from_mapped_field(
            avro_value_t *avro_value,
            const google::protobuf::Message &msg,
            const google::protobuf::FieldDescriptor *fd);

        static void assign_from_enum_field(
            avro_value_t *avro_value,
            const google::protobuf::EnumValueDescriptor *enum_value);
    };
}  // namespace avro
