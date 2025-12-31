/// -*- c++ -*-
//==============================================================================
/// @file avro-protobufvalue.c++
/// @brief Create Avro values from ProtoBuf messages
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufvalue.h++"
#include "avro-protobufschema.h++"
#include "avro-valuemethods.h++"
#include "avro-status.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"

namespace avro
{
    ProtoBufValue::ProtoBufValue(const google::protobuf::Message &msg)
        : CompoundValue(schema_from_proto(msg.GetDescriptor()))
    {
        this->assign_from_message(msg);
    }

    ProtoBufValue &ProtoBufValue::assign_from_message(
        const google::protobuf::Message &msg)
    {
        avro_value_reset(&this->value);
        This::assign_from_message(&this->value, msg);
        return *this;
    }

    void ProtoBufValue::assign_from_message(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg)
    {
        // assertf(avro_value_get_type(avro_value) == AVRO_RECORD,
        //         "Attempt to assign ProtoBuf message type %s to Avro non-record value type %s",
        //         msg.GetDescriptor()->full_name(),
        //         avro::type_name(avro_value));

        if (msg.GetDescriptor()->well_known_type())
        {
            This::assign_from_wellknown(avro_value, msg);
        }
        else if (auto *proto = dynamic_cast<const cc::protobuf::variant::Value *>(&msg))
        {
            avro::set_variant(
                avro_value,
                protobuf::decoded<core::types::Value>(*proto));
        }
        else if (auto *proto = dynamic_cast<const cc::protobuf::variant::ValueList *>(&msg))
        {
            avro::set_variant_list(
                avro_value,
                protobuf::decoded<core::types::ValueList>(*proto));
        }
        else if (auto *proto = dynamic_cast<const cc::protobuf::variant::KeyValueMap *>(&msg))
        {
            avro::set_variant_map(
                avro_value,
                protobuf::decoded<core::types::KeyValueMap>(*proto));
        }
        else
        {
            This::assign_from_custom(avro_value, msg);
        }
    }

    void ProtoBufValue::assign_from_wellknown(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg)
    {
        if (auto *proto = dynamic_cast<const google::protobuf::Timestamp *>(&msg))
        {
            avro::set_timestamp(avro_value, protobuf::decoded<core::dt::TimePoint>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Duration *>(&msg))
        {
            avro::set_time_interval(avro_value, protobuf::decoded<core::dt::Duration>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Value *>(&msg))
        {
            avro::set_variant(avro_value, protobuf::decoded<core::types::Value>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::ListValue *>(&msg))
        {
            avro::set_variant_list(avro_value, protobuf::decoded<core::types::ValueList>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Struct *>(&msg))
        {
            avro::set_variant_map(avro_value, protobuf::decoded<core::types::KeyValueMap>(*proto));
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::BoolValue *>(&msg))
        {
            avro::set_boolean(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::DoubleValue *>(&msg))
        {
            avro::set_double(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::FloatValue *>(&msg))
        {
            avro::set_float(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Int64Value *>(&msg))
        {
            avro::set_long(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::UInt64Value *>(&msg))
        {
            avro::set_long(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::Int32Value *>(&msg))
        {
            avro::set_int(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::UInt32Value *>(&msg))
        {
            avro::set_int(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::StringValue *>(&msg))
        {
            avro::set_string(avro_value, proto->value());
        }
        else if (auto *proto = dynamic_cast<const google::protobuf::BytesValue *>(&msg))
        {
            avro::set_bytes(avro_value, proto->value());
        }
        else
        {
            logf_warning("No known conversion from well-known ProtoBuf type %s",
                         msg.GetDescriptor()->full_name());
            avro::set_null(avro_value);
        }
    }

    void ProtoBufValue::assign_from_custom(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg)
    {
        const google::protobuf::Descriptor *descriptor = msg.GetDescriptor();
        int nfields = descriptor->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = descriptor->field(i);
            avro_value_t field_value = avro::get_field_by_index(*avro_value, i, fd->name());
            This::assign_from_field(&field_value, msg, fd);
        }
    }

    void ProtoBufValue::assign_from_field(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        if (fd->is_map())
        {
            This::assign_from_mapped_field(avro_value, msg, fd);
        }
        else if (fd->is_repeated())
        {
            This::assign_from_repeated_field(avro_value, msg, fd);
        }
        else if (fd->containing_oneof())
        {
            // This is an optional field, either because it is marked as
            // `optional` or because it is part of a `oneof` block. In either
            // case our Avro schema treats this as a union between a Null value
            // (discriminator index 0) and the actual field type (index 1).

            bool has_value = msg.GetReflection()->HasField(msg, fd);
            avro_value_t branch;

            if (has_value)
            {
                avro_value_set_branch(avro_value, 1, &branch);
                This::assign_from_single_field(&branch, msg, fd);
            }
            else
            {
                avro_value_set_branch(avro_value, 0, &branch);
                avro::set_null(&branch);
            }
        }
        else
        {
            This::assign_from_single_field(avro_value, msg, fd);
        }
    }

    void ProtoBufValue::assign_from_single_field(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        const google::protobuf::Reflection *reflection = msg.GetReflection();
        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            avro::set_int(avro_value, reflection->GetInt32(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            avro::set_long(avro_value, reflection->GetInt64(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            avro::set_int(avro_value, reflection->GetUInt32(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            avro::set_long(avro_value, reflection->GetUInt64(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            avro::set_double(avro_value, reflection->GetDouble(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            avro::set_float(avro_value, reflection->GetFloat(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            avro::set_boolean(avro_value, reflection->GetBool(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            This::assign_from_enum_field(avro_value, reflection->GetEnum(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            avro::set_string(avro_value, reflection->GetString(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            avro::set_bytes(avro_value, reflection->GetString(msg, fd));
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            This::assign_from_message(avro_value,
                                      reflection->GetMessage(msg, fd));
            break;

        default:
            logf_warning("No known Avro conversion for ProtoBuf message %s field %s",
                         msg.GetDescriptor()->full_name(),
                         fd->name());
            avro::set_null(avro_value);
            break;
        }
    }

    void ProtoBufValue::assign_from_indexed_field(
        avro_value_t *avro_value,
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
            avro::set_int(avro_value, reflection->GetRepeatedInt32(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            avro::set_long(avro_value, reflection->GetRepeatedInt64(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            avro::set_int(avro_value, reflection->GetRepeatedUInt32(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            avro::set_long(avro_value, reflection->GetRepeatedUInt64(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            avro::set_double(avro_value, reflection->GetRepeatedDouble(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            avro::set_float(avro_value, reflection->GetRepeatedFloat(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            avro::set_boolean(avro_value, reflection->GetRepeatedBool(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            This::assign_from_enum_field(
                avro_value,
                reflection->GetRepeatedEnum(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            avro::set_string(avro_value, reflection->GetRepeatedString(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            avro::set_bytes(avro_value, reflection->GetRepeatedString(msg, fd, index));
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            This::assign_from_message(avro_value,
                                      reflection->GetRepeatedMessage(msg, fd, index));
            break;

        default:
            logf_warning("No known Avro conversion for ProtoBuf message %s field %s",
                         msg.GetDescriptor()->full_name(),
                         fd->name());
            avro::set_null(avro_value);
            break;
        }
    }

    void ProtoBufValue::assign_from_repeated_field(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_ARRAY,
                "Attempt to assign repeated ProtoBuf value type %s to Avro non-array value type %s",
                fd->full_name(),
                avro::type_name(*avro_value));

        int size = msg.GetReflection()->FieldSize(msg, fd);
        for (int n = 0; n < size; n++)
        {
            avro_value_t element;
            checkstatus(avro_value_append(
                avro_value,  // value
                &element,    // child
                nullptr));   // new_index
            This::assign_from_indexed_field(&element, msg, fd, n);
            // TRY THIS:
            // avro_value_reset(&element);
            // avro_value_decref(&element);
        }
    }

    void ProtoBufValue::assign_from_mapped_field(
        avro_value_t *avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        const google::protobuf::Descriptor *map_descriptor = fd->message_type();
        const google::protobuf::FieldDescriptor *fd_key = map_descriptor->map_key();
        const google::protobuf::FieldDescriptor *fd_value = map_descriptor->map_value();

        assertf(fd_key->type() == google::protobuf::FieldDescriptor::TYPE_STRING,
                "Cannot convert ProtoBuf message %s map %s with non-string key type %s to Avro map",
                msg.GetDescriptor()->full_name(),
                fd->name(),
                fd_key->type_name());

        assertf(avro_value_get_type(avro_value) == AVRO_MAP,
                "Attempt to assign ProtoBuf map<%s, %s> to Avro non-map value type %s",
                fd_key->type_name(),
                fd_value->type_name(),
                avro::type_name(*avro_value));

        const google::protobuf::Reflection *reflection = msg.GetReflection();
        int size = reflection->FieldSize(msg, fd);
        for (int n = 0; n < size; n++)
        {
            const google::protobuf::Message &item = reflection->GetRepeatedMessage(msg, fd, n);
            std::string key = item.GetReflection()->GetString(item, fd_key);

            avro_value_t element;
            checkstatus(avro_value_add(
                avro_value,   // value
                key.c_str(),  // key
                &element,     // child
                nullptr,      // index
                nullptr));    // is_new

            This::assign_from_single_field(&element, item, fd_value);
        }
    }

    void ProtoBufValue::assign_from_enum_field(
            avro_value_t *avro_value,
            const google::protobuf::EnumValueDescriptor *enum_value)
    {
        // avro::set_enum_value(avro_value,
        //                      enum_value->index(),
        //                      enum_value->number());
        avro::set_enum(avro_value, enum_value->index());
    }

}  // namespace avro
