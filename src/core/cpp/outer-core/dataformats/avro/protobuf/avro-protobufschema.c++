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

namespace avro
{
    //--------------------------------------------------------------------------
    // ProtoBufSchema

    ProtoBufSchema::ProtoBufSchema(
        const ContextRef &context,
        const google::protobuf::Descriptor *descriptor)
        : RecordSchema(context, descriptor->name()),
          descriptor(descriptor)
    {
        this->add_fields();
    }

    void ProtoBufSchema::add_fields()
    {
        int n_fields = this->descriptor->field_count();

        for (int i = 0; i < n_fields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = this->descriptor->field(i);

            core::types::Value field_schema = This::field(fd);
            if (const google::protobuf::OneofDescriptor *ood = fd->containing_oneof())
            {
                // Avro does not have an exact counterpart to ProtoBuf `oneof`
                // fields. Specifically, an Avro Union is not suitable, since it
                // contains only (mutually exclusive) value types and no field
                // names. It would be impossible, for instance, to represents
                // two alternate fields of the same type. Therefore, we include
                // each field from the oneof block as separate Avro fields, but
                // with `null` as an alternate value type.
                field_schema = core::types::ValueList({TypeName_Null, field_schema});
            }

            this->add_field(fd->name(), field_schema, This::field_comment(fd));
        }
    }

    core::types::Value ProtoBufSchema::field(
        const google::protobuf::FieldDescriptor *fd) const
    {
        core::types::Value schema;
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

        return schema;
    }

    core::types::Value ProtoBufSchema::field_schema(
        const google::protobuf::FieldDescriptor *fd) const
    {
        core::types::Value schema;

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
            schema = This::from_descriptor(this->context, fd->message_type());
            break;

        default:
            schema = TypeName_Null;
            break;
        }
        return schema;
    }

    EnumSchema ProtoBufSchema::enum_schema(
        const google::protobuf::EnumDescriptor *ed,
        const google::protobuf::EnumValueDescriptor *default_value) const
    {
        return {
            this->context,
            ed->name(),
            protobuf::enum_names(ed),
            protobuf::enum_name(default_value->number(), ed)};
    }

    MapSchema ProtoBufSchema::map_schema(
        const google::protobuf::Descriptor *md) const
    {
        return This::field_schema(md->map_value());
    }

    SchemaWrapper ProtoBufSchema::from_descriptor(
        const ContextRef &context,
        const google::protobuf::Descriptor *descriptor)
    {
        switch (descriptor->well_known_type())
        {
        case google::protobuf::Descriptor::WELLKNOWNTYPE_DOUBLEVALUE:
            return SchemaWrapper(TypeName_Double);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_FLOATVALUE:
            return SchemaWrapper(TypeName_Float);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT64VALUE:
        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT64VALUE:
            return SchemaWrapper(TypeName_Long);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT32VALUE:
        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT32VALUE:
            return SchemaWrapper(TypeName_Int);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRINGVALUE:
            return SchemaWrapper(TypeName_String);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BYTESVALUE:
            return SchemaWrapper(TypeName_Bytes);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BOOLVALUE:
            return SchemaWrapper(TypeName_Boolean);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_DURATION:
            return TimeIntervalSchema(context);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP:
            return TimestampSchema(context);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_VALUE:
            return VariantSchema(context);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_LISTVALUE:
            return VariantListSchema(context);

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRUCT:
            return VariantMapSchema(context);

        default:
            if (descriptor == cc::protobuf::variant::Value::GetDescriptor())
            {
                return VariantSchema(context);
            }
            else if (descriptor == cc::protobuf::variant::KeyValueMap::GetDescriptor())
            {
                return VariantMapSchema(context);
            }
            else if (descriptor == cc::protobuf::variant::ValueList::GetDescriptor())
            {
                return VariantListSchema(context);
            }
            else
            {
                return ProtoBufSchema(context, descriptor);
            }
        }
    }

    std::optional<std::string> ProtoBufSchema::field_comment(
        const google::protobuf::FieldDescriptor *fd)
    {
        google::protobuf::SourceLocation source;
        if (fd->GetSourceLocation(&source))
        {
            return source.leading_comments;
        }
        else
        {
            return {};
        }
    }

    //--------------------------------------------------------------------------
    // schema_from_proto (Entry point)

    SchemaWrapper &schema_from_proto(const google::protobuf::Descriptor *descriptor)
    {
        using SchemaMap = std::unordered_map<
            const google::protobuf::Descriptor *,
            SchemaWrapper>;

        static SchemaMap schema_map;
        static std::mutex mtx;


        if (schema_map.count(descriptor) == 0)
        {
            logf_debug("schema_from_proto(%s) miss; creating", descriptor->full_name());
            auto context = std::make_shared<BuilderContext>();
            SchemaWrapper schema = ProtoBufSchema::from_descriptor(context, descriptor);

            std::scoped_lock lock(mtx);
            schema_map.insert_or_assign(descriptor, std::move(schema));
        }

        return schema_map.at(descriptor);
    }

}  // namespace avro
