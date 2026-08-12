/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message-encoder.c++
/// @brief Populate ProtoBuf message from variant Value instances
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-message-encoder.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-enum.h++"
#include "protobuf-inline.h++"
#include "status/exceptions.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"

namespace cc::protobuf
{
    constexpr bool ENUMS_AS_STRINGS = true;

    bool MessageEncoder::encode_message(
        const core::types::Value& value,
        ::google::protobuf::Message* msg)
    {
        const ::google::protobuf::Descriptor* descriptor = msg->GetDescriptor();
        bool encoded = false;

        if (msg->GetDescriptor()->well_known_type())
        {
            encoded = This::encode_wellknown(value, msg);
        }

        else if (auto* proto_value = dynamic_cast<cc::protobuf::variant::Value*>(msg))
        {
            encode(value, proto_value);
            encoded = true;
        }

        else if (auto* proto_value = dynamic_cast<cc::protobuf::variant::ValueList*>(msg))
        {
            if (auto vlist = value.get_valuelist_ptr())
            {
                encode(*vlist, proto_value);
                encoded = true;
            }
        }

        else if (auto* proto_value = dynamic_cast<cc::protobuf::variant::TaggedValueList*>(msg))
        {
            if (auto tvlist = value.get_tvlist_ptr())
            {
                encode(*tvlist, proto_value);
                encoded = true;
            }
        }

        else if (auto* proto_value = dynamic_cast<cc::protobuf::variant::KeyValueMap*>(msg))
        {
            if (auto kvmap = value.get_kvmap_ptr())
            {
                encode(*kvmap, proto_value);
                encoded = true;
            }
        }

        else
        {
            encoded = This::encode_custom(value, msg);
        }

        return encoded;
    }

    bool MessageEncoder::encode_wellknown(
        const core::types::Value& value,
        ::google::protobuf::Message* msg)
    {
        if (auto* proto_value = dynamic_cast<google::protobuf::Duration*>(msg))
        {
            encode(value.as_duration(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Timestamp*>(msg))
        {
            encode(value.as_timepoint(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Timestamp*>(msg))
        {
            encode(value.as_timepoint(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::DoubleValue*>(msg))
        {
            encode(value.as_double(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::FloatValue*>(msg))
        {
            encode(value.as_float(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Int64Value*>(msg))
        {
            encode(value.as_sint64(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::UInt64Value*>(msg))
        {
            encode(value.as_uint64(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Int32Value*>(msg))
        {
            encode(value.as_sint32(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::UInt32Value*>(msg))
        {
            encode(value.as_uint32(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::StringValue*>(msg))
        {
            encode(value.as_string(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::BytesValue*>(msg))
        {
            encode(value.as_bytevector(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::BoolValue*>(msg))
        {
            encode(value.as_bool(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Value*>(msg))
        {
            encode(value, proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::ListValue*>(msg))
        {
            encode(value.as_valuelist(), proto_value);
        }

        else if (auto* proto_value = dynamic_cast<google::protobuf::Struct*>(msg))
        {
            encode(value.as_kvmap(), proto_value);
        }
        else
        {
            return false;
        }

        return true;
    }

    bool MessageEncoder::encode_custom(
        const core::types::Value& value,
        ::google::protobuf::Message* msg)
    {
        const google::protobuf::Descriptor* descriptor = msg->GetDescriptor();
        int nfields = descriptor->field_count();
        bool all_succeeded = true;
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor* fd = descriptor->field(i);

            if (const core::types::Value& field_value = value.get(fd->name()))
            {
                bool success = This::encode_field(field_value, msg, fd);
                if (!success)
                {
                    logf_notice(
                        "%s: Cannot reconstruct ProtoBuf %s field %r of type %s from value type %s: %r",
                        TYPE_NAME_FULL(This),
                        msg->GetDescriptor()->full_name(),
                        fd->name(),
                        fd->type_name(),
                        field_value.type_name(),
                        field_value);
                    all_succeeded = false;
                }
            }
            else if (!fd->is_optional() && !fd->containing_oneof())
            {
                logf_notice(
                    "%s: Expected %s field %r not present in input value: %s",
                    TYPE_NAME_FULL(This),
                    msg->GetDescriptor()->full_name(),
                    fd->name(),
                    value);
                all_succeeded = false;
            }
        }
        return all_succeeded;
    }

    bool MessageEncoder::encode_field(
        const core::types::Value& field_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        if (fd->is_map())
        {
            return This::encode_mapped_field(field_value, msg, fd);
        }
        else if (fd->is_repeated())
        {
            return This::encode_repeated_field(field_value, msg, fd);
        }
        else
        {
            return This::encode_single_field(field_value, msg, fd);
        }
    }

    bool MessageEncoder::encode_single_field(
        const core::types::Value& value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        const google::protobuf::Reflection* reflection = msg->GetReflection();
        bool encoded = false;

        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            if (auto opt = value.try_as_sint32())
            {
                reflection->SetInt32(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            if (auto opt = value.try_as_sint64())
            {
                reflection->SetInt64(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            if (auto opt = value.try_as_uint32())
            {
                reflection->SetUInt32(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            if (auto opt = value.try_as_uint64())
            {
                reflection->SetUInt64(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            if (auto opt = value.try_as_double())
            {
                reflection->SetDouble(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            if (auto opt = value.try_as_float())
            {
                reflection->SetFloat(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            if (auto opt = value.try_as_bool())
            {
                reflection->SetBool(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            if (auto opt = This::encode_enum_value(value, fd->enum_type()))
            {
                reflection->SetEnumValue(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            if (value.is_simple())
            {
                reflection->SetString(msg, fd, value.as_string());
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            if (auto *bytevector = value.get_if<core::types::ByteVector>())
            {
                reflection->SetString(msg, fd, bytevector->as_string());
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            encoded = This::encode_message(
                value,
                reflection->MutableMessage(msg, fd));
            break;
        }

        return encoded;
    }

    bool MessageEncoder::encode_repeated_field(
        const core::types::Value& value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        if (auto vlist = value.get_valuelist_ptr())
        {
            bool encoded = true;
            for (const core::types::Value& element : *vlist)
            {
                encoded &= This::encode_repeated_element(
                    element,
                    msg,
                    fd);
            }
            return encoded;
        }
        else
        {
            return false;
        }
    }

    bool MessageEncoder::encode_repeated_element(
        const core::types::Value& value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        const google::protobuf::Reflection* reflection = msg->GetReflection();
        bool encoded = false;

        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            if (auto opt = value.try_as_sint32())
            {
                reflection->AddInt32(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            if (auto opt = value.try_as_sint64())
            {
                reflection->AddInt64(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            if (auto opt = value.try_as_uint32())
            {
                reflection->AddUInt32(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            if (auto opt = value.try_as_uint64())
            {
                reflection->AddUInt64(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            if (auto opt = value.try_as_double())
            {
                reflection->AddDouble(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            if (auto opt = value.try_as_float())
            {
                reflection->AddFloat(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            if (auto opt = value.try_as_bool())
            {
                reflection->AddBool(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            if (auto opt = This::encode_enum_value(value, fd->enum_type()))
            {
                reflection->AddEnumValue(msg, fd, *opt);
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            if (value.is_simple())
            {
                reflection->AddString(msg, fd, value.as_string());
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            if (auto *bytevector = value.get_if<core::types::ByteVector>())
            {
                reflection->AddString(msg, fd, bytevector->as_string());
                encoded = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            encoded = This::encode_message(
                value,
                reflection->AddMessage(msg, fd));
            break;
        }

        return encoded;
    }

    bool MessageEncoder::encode_mapped_field(
        const core::types::Value& value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        const google::protobuf::Descriptor* map_descriptor = fd->message_type();
        const google::protobuf::FieldDescriptor* fd_key = map_descriptor->map_key();
        const google::protobuf::FieldDescriptor* fd_value = map_descriptor->map_value();

        if (value.is_mappable() &&
            (fd_key->type() == google::protobuf::FieldDescriptor::TYPE_STRING))
        {
            const google::protobuf::Reflection* reflection = msg->GetReflection();
            bool encoded = true;
            for (const auto& [key, value] : value.as_kvmap())
            {
                google::protobuf::Message* item_message = reflection->AddMessage(msg, fd);
                const google::protobuf::Reflection* item_reflection = item_message->GetReflection();

                item_reflection->SetString(item_message, fd_key, key);
                encoded &= This::encode_message(
                    value,
                    item_reflection->MutableMessage(item_message, fd_value));
            }
            return encoded;
        }
        else
        {
            return false;
        }
    }

    std::optional<int> MessageEncoder::encode_enum_value(
        const core::types::Value& value,
        const google::protobuf::EnumDescriptor* ed)
    {
        if (value.is_integral())
        {
            return value.as_sint();
        }
        else if (value.is_string())
        {
            return enum_value(value.as_string(), ed);
        }
        else
        {
            return {};
        }
    }

    bool encode_to_message(
        const core::types::Value& value,
        ::google::protobuf::Message* msg)
    {
        return MessageEncoder::encode_message(value, msg);
    }
}  // namespace cc::protobuf
