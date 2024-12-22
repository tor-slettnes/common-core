/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.c++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "avro-protobufvalue.h++"
#include "avro-protobufschema.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"

namespace avro
{
    ProtoBufValue::ProtoBufValue(const google::protobuf::Message &msg)
        : CompoundValue(schema_from_proto(msg.GetDescriptor()))
    {
        This::assign_from_message(&this->value, msg);
    }

    void ProtoBufValue::assign_from_message(
        avro_value_t *value,
        const google::protobuf::Message &msg)
    {
        assertf(avro_value_get_type(value) == AVRO_RECORD,
                "Attempt to assign repepated ProtoBuf value to Avro non-record value type %s",
                This::type_name(value));

        if (msg.GetDescriptor()->well_known_type())
        {
            This::assign_from_wellknown(value, msg);
        }
        else if (auto *proto = dynamic_cast<const cc::variant::Value *>(&msg))
        {
            This::set_variant(
                value,
                protobuf::decoded<core::types::Value>(*proto));
        }
        else if (auto *proto = dynamic_cast<const cc::variant::ValueList *>(&msg))
        {
            This::set_variant_list(
                value,
                protobuf::decoded<core::types::ValueList>(*proto));
        }
        else if (auto *proto = dynamic_cast<const cc::variant::KeyValueMap *>(&msg))
        {
            This::set_variant_map(
                value,
                protobuf::decoded<core::types::KeyValueMap>(*proto));
        }
        else
        {
            This::assign_from_custom(value, msg);
        }
    }

    void ProtoBufValue::assign_from_wellknown(
        avro_value_t *value,
        const google::protobuf::Message &msg)
    {
        if (auto *proto = dynamic_cast<const google::protobuf::Duration *>(&msg))
        {
            This::set_duration(value, protobuf::decoded<core::dt::Duration>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Timestamp *>(&msg))
        {
            This::set_timestamp(value, protobuf::decoded<core::dt::TimePoint>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Value *>(&msg))
        {
            This::set_variant(value, protobuf::decoded<core::types::Value>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::ListValue *>(&msg))
        {
            This::set_variant_list(value, protobuf::decoded<core::types::ValueList>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Struct *>(&msg))
        {
            This::set_variant_map(value, protobuf::decoded<core::types::KeyValueMap>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::BoolValue *>(&msg))
        {
            This::set_boolean(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::DoubleValue *>(&msg))
        {
            This::set_double(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::FloatValue *>(&msg))
        {
            This::set_float(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Int64Value *>(&msg))
        {
            This::set_long(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::UInt64Value *>(&msg))
        {
            This::set_long(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Int32Value *>(&msg))
        {
            This::set_int(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::UInt32Value *>(&msg))
        {
            This::set_int(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::StringValue *>(&msg))
        {
            This::set_string(value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::BytesValue *>(&msg))
        {
            This::set_bytes(value, proto->value());
        }
        else
        {
            logf_warning("No known conversion from well-known ProtoBuf type %s",
                         msg.GetDescriptor()->full_name());
            This::set_null(value);
        }
    }

    void ProtoBufValue::assign_from_custom(
        avro_value_t *value,
        const google::protobuf::Message &msg)
    {
        const google::protobuf::Descriptor *descriptor = msg.GetDescriptor();
        int nfields = descriptor->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = descriptor->field(i);
            avro_value_t field_value = This::get_by_index(value, i, fd->name());
            This::assign_from_field(&field_value, msg, fd);
        }
    }

    void ProtoBufValue::assign_from_field(
        avro_value_t *value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        if (fd->is_map())
        {
            This::assign_from_mapped_field(value, msg, fd);
        }
        else if (fd->is_repeated())
        {
            This::assign_from_repeated_field(value, msg, fd);
        }
        else if (fd->containing_oneof())
        {
            // This is an optional field, either because it is marked as
            // `optional` or because it is part of a `oneof` block. In either
            // case our Avro schema treats this as a union between a Null value
            // (discriminator index 0) and the actual field type (index 1).

            bool has_value = msg.GetReflection()->HasField(msg, fd);
            avro_value_t branch;
            avro_value_set_branch(value,              // value
                                  has_value ? 1 : 0,  // discriminant
                                  &branch);           // branch
            if (has_value)
            {
                This::assign_from_single_field(&branch, msg, fd);
            }
            else
            {
                This::set_null(&branch);
            }
        }
        else
        {
            This::assign_from_single_field(value, msg, fd);
        }
    }

    void ProtoBufValue::assign_from_single_field(
        avro_value_t *value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        const google::protobuf::Reflection *reflection = msg.GetReflection();
        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            This::set_int(value, reflection->GetInt32(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            This::set_long(value, reflection->GetInt64(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            This::set_int(value, reflection->GetUInt32(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            This::set_long(value, reflection->GetUInt64(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            This::set_double(value, reflection->GetDouble(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            This::set_float(value, reflection->GetFloat(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            This::set_boolean(value, reflection->GetBool(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            This::set_enum(value, reflection->GetEnumValue(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            This::set_string(value, reflection->GetString(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            This::set_bytes(value, reflection->GetString(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            This::assign_from_message(value,
                                      reflection->GetMessage(msg, fd));
            break;

        default:
            logf_warning("No known Avro conversion for ProtoBuf message %s field %s",
                         msg.GetDescriptor()->full_name(),
                         fd->name());
            This::set_null(value);
            break;
        }
    }

    void ProtoBufValue::assign_from_indexed_field(
        avro_value_t *value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd,
        int index)
    {
        const google::protobuf::Reflection *reflection = msg.GetReflection();

        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            This::set_int(value, reflection->GetRepeatedInt32(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            This::set_long(value, reflection->GetRepeatedInt64(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            This::set_int(value, reflection->GetRepeatedUInt32(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            This::set_long(value, reflection->GetRepeatedUInt64(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            This::set_double(value, reflection->GetRepeatedDouble(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            This::set_float(value, reflection->GetRepeatedFloat(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            This::set_boolean(value, reflection->GetRepeatedBool(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            This::set_enum(value, reflection->GetRepeatedEnumValue(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            This::set_string(value, reflection->GetRepeatedString(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            This::set_bytes(value, reflection->GetRepeatedString(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            This::assign_from_message(value,
                                      reflection->GetRepeatedMessage(msg, fd, index));
            break;

        default:
            logf_warning("No known Avro conversion for ProtoBuf message %s field %s",
                         msg.GetDescriptor()->full_name(),
                         fd->name());
            This::set_null(value);
            break;
        }
    }

    void ProtoBufValue::assign_from_repeated_field(
        avro_value_t *value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        assertf(avro_value_get_type(value) == AVRO_ARRAY,
                "Attempt to assign repepated ProtoBuf value to Avro non-array value type %s",
                This::type_name(value));

        int size = msg.GetReflection()->FieldSize(msg, fd);
        for (int n = 0; n < size; n++)
        {
            avro_value_t element;
            checkstatus(avro_value_append(
                value,      // value
                &element,   // child
                nullptr));  // new_index
            This::assign_from_indexed_field(&element, msg, fd, n);
            // TRY THIS:
            // avro_value_reset(&element);
            // avro_value_decref(&element);
        }
    }

    void ProtoBufValue::assign_from_mapped_field(
        avro_value_t *value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        assertf(avro_value_get_type(value) == AVRO_MAP,
                "Attempt to assign repepated ProtoBuf value to Avro non-map value type %s",
                This::type_name(value));

        const google::protobuf::Descriptor *map_descriptor = fd->message_type();
        const google::protobuf::FieldDescriptor *fd_key = map_descriptor->map_key();
        const google::protobuf::FieldDescriptor *fd_value = map_descriptor->map_value();

        assertf(fd_key->type() == google::protobuf::FieldDescriptor::TYPE_STRING,
                "Cannot convert ProtoBuf message %s map %s with non-string key type %s to Avro map",
                msg->full_name(),
                fd->name(),
                fd_key->type_name());

        const google::protobuf::Reflection *reflection = msg.GetReflection();
        int size = reflection->FieldSize(msg, fd);
        for (int n = 0; n < size; n++)
        {
            const google::protobuf::Message &item = reflection->GetRepeatedMessage(msg, fd, n);
            std::string key = item.GetReflection()->GetString(item, fd_key);

            avro_value_t element;
            checkstatus(avro_value_add(
                value,        // value
                key.c_str(),  // key
                &element,     // child
                nullptr,      // index
                nullptr));    // is_new

            This::assign_from_single_field(&element, item, fd_value);
        }
    }
}  // namespace avro
