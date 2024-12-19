/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.c++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-protobufvalue.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"

namespace core::avro
{
    avro_schema_t ProtoBufValue::schema(
        const google::protobuf::Descriptor *descriptor)
    {
        try
        {
            return This::standard_schema_map.at(descriptor)();
        }
        catch (const std::out_of_range &)
        {
            return This::custom_schema(descriptor);
        }
    }

    avro_schema_t ProtoBufValue::custom_schema(
        const google::protobuf::Descriptor *descriptor)
    {
        return nullptr;
    }

    // types::KeyValueMap ProtoBufValue::message_as_kvmap(
    //     const google::protobuf::Descriptor *descriptor)
    // {
    //     int n_fields = descriptor->field_count();

    //     for (int i = 0; i < n_fields; i++)
    //     {
    //         google::protobuf::FieldDescriptor *fd = descriptor->field(i);

    //         if (const google::protobuf::OnefDescriptor *ood = fd->containing_oneof())
    //         {
    //         }
    //     }

    //     int n_oneofs = descriptor->oneof_decl_count();
    //     for (i = 0; i < n_oneofs; i++)
    //     {
    //         const OneofDescriptor *ood = oneof_decl(i);
    //     }
    // }

    // types::KeyValueMap ProtoBufValue::field_as_kvmap(
    //     const google::protobuf::FieldDescriptor &fd)
    // {
    // }

    // types::KeyValueMap ProtoBufValue::single_field_as_kvmap(
    //     const google::protobuf::FieldDescriptor &fd)
    // {
    // }

    // types::KeyValueMap ProtoBufValue::repeated_field_as_list(
    //     const google::protobuf::FieldDescriptor &fd)
    // {
    // }

    const ProtoBufValue::SchemaMap ProtoBufValue::standard_schema_map = {
        {google::protobuf::Empty::GetDescriptor(), avro_schema_null},
        {google::protobuf::BoolValue::GetDescriptor(), avro_schema_boolean},
        {google::protobuf::Int32Value::GetDescriptor(), avro_schema_int},
        {google::protobuf::UInt32Value::GetDescriptor(), avro_schema_int},
        {google::protobuf::Int64Value::GetDescriptor(), avro_schema_long},
        {google::protobuf::UInt64Value::GetDescriptor(), avro_schema_long},
        {google::protobuf::FloatValue::GetDescriptor(), avro_schema_float},
        {google::protobuf::DoubleValue::GetDescriptor(), avro_schema_double},
        {google::protobuf::StringValue::GetDescriptor(), avro_schema_string},
        {google::protobuf::BytesValue::GetDescriptor(), avro_schema_bytes},
        {google::protobuf::Timestamp::GetDescriptor(), This::timestamp_schema},
        {google::protobuf::Duration::GetDescriptor(), This::duration_schema},
        {google::protobuf::Value::GetDescriptor(), This::variant_value_schema},
        {google::protobuf::Struct::GetDescriptor(), This::variant_map_schema},
        {google::protobuf::ListValue::GetDescriptor(), This::variant_list_schema},
        {cc::variant::Value::GetDescriptor(), This::variant_value_schema},
        {cc::variant::ValueList::GetDescriptor(), This::variant_list_schema},
    };
} // namespace core::avro
