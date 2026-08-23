/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufschema.c++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufschema.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-enum.h++"
#include "string/misc.h++"
#include "logging/logging.h++"

namespace cc::avro
{
    const std::unordered_set<std::string> NAMESPACE_REMOVALS = {
        "protobuf",
    };

    //--------------------------------------------------------------------------
    // ProtoBufSchema

    ProtoBufSchema::ProtoBufSchema(
        const ContextRef &context,
        const google::protobuf::Descriptor *descriptor)
        : RecordSchema(context, This::schema_name(descriptor)),
          descriptor(descriptor)
    {
        this->add_fields();
    }

    const SchemaWrapper &ProtoBufSchema::from_proto(
        const google::protobuf::Descriptor *descriptor)
    {
        static SchemaMap cached_schemas;
        try
        {
            return cached_schemas.at(descriptor);
        }
        catch (std::out_of_range)
        {
            auto context = std::make_shared<BuilderContext>();
            FieldData schema_data = This::from_descriptor(context, descriptor);
            auto [it, inserted] = cached_schemas.insert_or_assign(
                descriptor,
                schema_data.schema);

            return it->second;
        }
    }

    const SchemaWrapper &ProtoBufSchema::from_proto_with_envelope(
        const google::protobuf::Descriptor *contents_descriptor,
        const google::protobuf::Descriptor *envelope_descriptor,
        const std::optional<std::string> &schema_name)
    {
        static SchemaMap cached_schemas;

        try
        {
            return cached_schemas.at(contents_descriptor);
        }
        catch (std::out_of_range)
        {
            auto context = std::make_shared<BuilderContext>();
            RecordSchema wrapper(
                context,
                schema_name.value_or(
                    This::schema_name(contents_descriptor) + "Message"));

            wrapper.add_field(
                ENVELOPE_FIELD,
                This::from_descriptor(context, envelope_descriptor).schema);

            wrapper.add_field(
                CONTENTS_FIELD,
                This::from_descriptor(context, contents_descriptor).schema);

            auto [it, inserted] = cached_schemas.insert_or_assign(
                contents_descriptor,
                wrapper);

            return it->second;
        }
    }

    void ProtoBufSchema::add_fields()
    {
        int n_fields = this->descriptor->field_count();

        for (int i = 0; i < n_fields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = this->descriptor->field(i);

            FieldData field = This::field(fd);
            if (const google::protobuf::OneofDescriptor *ood = fd->containing_oneof())
            {
                // Avro does not have an exact counterpart to ProtoBuf `oneof`
                // fields. Specifically, an Avro Union is not suitable, since it
                // contains only (mutually exclusive) value types and no field
                // names. It would be impossible, for instance, to represents
                // two alternate fields of the same type. Therefore, we include
                // each field from the oneof block as separate Avro fields, but
                // with `null` as an alternate value type.
                core::types::ValueList alternates;
                alternates.push_back(TypeName_Null);
                alternates.push_back(field.schema);
                field.schema = SchemaWrapper(alternates);
                field.default_value = core::types::Value();
            }

            this->add_field(fd->name(),
                            field.schema,
                            field.default_value,
                            This::field_comment(fd));
        }
    }

    ProtoBufSchema::FieldData ProtoBufSchema::field(
        const google::protobuf::FieldDescriptor *fd)
    {
        FieldData field;

        if (fd->is_map())
        {
            field = This::map_schema(fd->message_type());
        }
        else
        {
            field = This::field_schema(fd);

            if (fd->is_repeated())
            {
                field.schema = ArraySchema(field.schema);
                field.default_value = core::types::ValueList();
            }
        }

        return field;
    }

    ProtoBufSchema::FieldData ProtoBufSchema::field_schema(
        const google::protobuf::FieldDescriptor *fd)
    {
        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            return {SchemaWrapper(TypeName_Int), 0};

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            return {SchemaWrapper(TypeName_Long), 0};

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            return {SchemaWrapper(TypeName_Double), 0.0};

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            return {SchemaWrapper(TypeName_Float), 0.0};

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            return {SchemaWrapper(TypeName_Boolean), false};

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            return This::enum_schema(
                fd->enum_type(),
                fd->default_value_enum());

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            return {SchemaWrapper(TypeName_String), ""s};

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            return {SchemaWrapper(TypeName_Bytes), ""s};

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            if (const google::protobuf::Descriptor *md = fd->message_type())
            {
                std::string schema_name = This::schema_name(md);
                if (this->context->defined_schemas.count(schema_name))
                {
                    return {SchemaWrapper(schema_name), {}};
                }
                else
                {
                    return This::from_descriptor(this->context, md);
                }
            }
            break;
        }

        return {SchemaWrapper(TypeName_Null), {}};
    }

    ProtoBufSchema::FieldData ProtoBufSchema::enum_schema(
        const google::protobuf::EnumDescriptor *ed,
        const google::protobuf::EnumValueDescriptor *default_value)
    {
        return {
            EnumSchema(
                this->context,
                This::translated_namespace(ed->full_name()),
                cc::protobuf::enum_names(ed)),
            cc::protobuf::enum_name(default_value->number(), ed),
        };
    }

    ProtoBufSchema::FieldData ProtoBufSchema::map_schema(
        const google::protobuf::Descriptor *md)
    {
        return {
            MapSchema(This::field_schema(md->map_value()).schema),
            core::types::KeyValueMap(),
        };
    }

    ProtoBufSchema::FieldData ProtoBufSchema::from_descriptor(
        const ContextRef &context,
        const google::protobuf::Descriptor *descriptor)
    {
        switch (descriptor->well_known_type())
        {
        case google::protobuf::Descriptor::WELLKNOWNTYPE_DOUBLEVALUE:
            return {SchemaWrapper(TypeName_Double), 0.0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_FLOATVALUE:
            return {SchemaWrapper(TypeName_Float), 0.0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT64VALUE:
        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT64VALUE:
            return {SchemaWrapper(TypeName_Long), 0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT32VALUE:
        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT32VALUE:
            return {SchemaWrapper(TypeName_Int), 0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRINGVALUE:
            return {SchemaWrapper(TypeName_String), ""s};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BYTESVALUE:
            return {SchemaWrapper(TypeName_Bytes), ""s};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BOOLVALUE:
            return {SchemaWrapper(TypeName_Boolean), false};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_DURATION:
            return {TimeIntervalSchema(context), 0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP:
            return {TimestampSchema(context), 0};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_VALUE:
            return {VariantSchema(context), core::types::Value()};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_LISTVALUE:
            return {VariantListSchema(context), core::types::ValueList()};

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRUCT:
            return {VariantMapSchema(context), core::types::KeyValueMap()};

        default:
            if (descriptor == cc::protobuf::variant::Value::GetDescriptor())
            {
                return {VariantSchema(context), core::types::Value()};
            }
            else if (descriptor == cc::protobuf::variant::KeyValueMap::GetDescriptor())
            {
                return {VariantMapSchema(context), core::types::KeyValueMap()};
            }
            else if (descriptor == cc::protobuf::variant::ValueList::GetDescriptor())
            {
                return {VariantListSchema(context), core::types::ValueList()};
            }
            else
            {
                return {ProtoBufSchema(context, descriptor), {}};
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

    std::string ProtoBufSchema::schema_name(
        const google::protobuf::Descriptor *descriptor)
    {
        return This::translated_namespace(descriptor->full_name());
    }

    std::string ProtoBufSchema::translated_namespace(
        const std::string &protobuf_namespace)
    {
        std::vector<std::string> parts = core::str::split(protobuf_namespace, ".");

        for (auto it = parts.begin(); it != parts.end();)
        {
            if (NAMESPACE_REMOVALS.count(*it))
            {
                it = parts.erase(it);
            }
            else
            {
                it++;
            }
        }
        return core::str::join(parts, ".");
    }

}  // namespace cc::avro
