/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.c++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-protobufschema.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-enum.h++"
#include "logging/logging.h++"

namespace core::avro
{
    //--------------------------------------------------------------------------
    // ProtoBufSchemaBuilder

    ProtoBufSchemaBuilder::ProtoBufSchemaBuilder(
        const google::protobuf::Descriptor *descriptor)
        : RecordSchema(descriptor->name(),
                       This::fields(descriptor))
    {
        logf_trace("ProtoBufSchemaBuilder constructor for %r: %s",
                   descriptor->name(),
                   *this);
    }

    types::ValueList ProtoBufSchemaBuilder::fields(
        const google::protobuf::Descriptor *descriptor)
    {
        types::ValueList fields;
        int n_fields = descriptor->field_count();

        for (int i = 0; i < n_fields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = descriptor->field(i);
            if (const google::protobuf::OneofDescriptor *ood = fd->containing_oneof())
            {
                // Avro does not have an exact counterpart to ProtoBuf `oneof`
                // fields. Specifically, an Avro Union is not suitable, since it
                // contains only mutually exclusive value types and no field
                // names. Therefore, we include each field within the oneof
                // block separately, but with `null` as an alternate value type.

                fields.push_back(This::field(fd, true));
            }
            else
            {
                fields.push_back(This::field(fd, false));
            }
        }
        return fields;
    }

    RecordField ProtoBufSchemaBuilder::field(
        const google::protobuf::FieldDescriptor *fd,
        bool is_optional)
    {
        types::Value schema;
        if (fd->is_map())
        {
            schema = This::map_schema(fd->message_type());
        }
        else
        {
            schema = This::field_schema(fd);
        }

        if (fd->is_repeated())
        {
            schema = ArraySchema(schema);
        }

        if (is_optional)
        {
            schema = types::ValueList({TypeName_Null, schema});
        }

        return {schema, fd->name()};
    }

    types::Value ProtoBufSchemaBuilder::field_schema(
        const google::protobuf::FieldDescriptor *fd)
    {
        types::Value schema;

        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            schema = TypeName_Int;
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            schema = TypeName_Long;
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            schema = TypeName_Double;
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            schema = TypeName_Float;
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            schema = TypeName_Boolean;
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            schema = This::enum_schema(fd->enum_type(), fd->default_value_enum());
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            schema = TypeName_String;
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            schema = TypeName_Bytes;
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            schema = schema_from_proto(fd->message_type());
            break;

        default:
            schema = TypeName_Null;
            break;
        }
        return schema;
    }

    EnumSchema ProtoBufSchemaBuilder::enum_schema(
        const google::protobuf::EnumDescriptor *ed,
        const google::protobuf::EnumValueDescriptor *default_value)
    {
        return {
            ed->name(),
            protobuf::enum_names(ed),
            protobuf::enum_name(default_value->number(), ed)};
    }

    MapSchema ProtoBufSchemaBuilder::map_schema(
        const google::protobuf::Descriptor *md)
    {
        return This::field_schema(md->map_value());
    }

    //--------------------------------------------------------------------------
    // schema_from_proto (Entry point)

    SchemaWrapper schema_from_proto(const google::protobuf::Descriptor *descriptor)
    {
        using SchemaMap = std::unordered_map<const google::protobuf::Descriptor *,
                                             SchemaWrapper>;

        static const SchemaMap standard_schema_map = {
            {google::protobuf::Empty::GetDescriptor(), SchemaWrapper(TypeName_Null)},
            {google::protobuf::BoolValue::GetDescriptor(), SchemaWrapper(TypeName_Boolean)},
            {google::protobuf::Int32Value::GetDescriptor(), SchemaWrapper(TypeName_Int)},
            {google::protobuf::UInt32Value::GetDescriptor(), SchemaWrapper(TypeName_Int)},
            {google::protobuf::Int64Value::GetDescriptor(), SchemaWrapper(TypeName_Long)},
            {google::protobuf::UInt64Value::GetDescriptor(), SchemaWrapper(TypeName_Long)},
            {google::protobuf::FloatValue::GetDescriptor(), SchemaWrapper(TypeName_Float)},
            {google::protobuf::DoubleValue::GetDescriptor(), SchemaWrapper(TypeName_Double)},
            {google::protobuf::StringValue::GetDescriptor(), SchemaWrapper(TypeName_String)},
            {google::protobuf::BytesValue::GetDescriptor(), SchemaWrapper(TypeName_String)},
            {google::protobuf::Timestamp::GetDescriptor(), DurationSchema(TypeName_Timestamp)},
            {google::protobuf::Duration::GetDescriptor(), DurationSchema(TypeName_Duration)},
            {google::protobuf::Value::GetDescriptor(), VariantSchema()},
            {google::protobuf::Struct::GetDescriptor(), MapSchema(TypeName_Variant)},
            {google::protobuf::ListValue::GetDescriptor(), ArraySchema(TypeName_Variant)},
            {cc::variant::Value::GetDescriptor(), VariantSchema()},
            {cc::variant::ValueList::GetDescriptor(), ArraySchema(TypeName_Variant)},
        };

        try
        {
            return standard_schema_map.at(descriptor);
        }
        catch (const std::out_of_range &)
        {
            return ProtoBufSchemaBuilder(descriptor);
        }
    }

}  // namespace core::avro
