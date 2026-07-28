/// -*- c++ -*-
//==============================================================================
/// @file avro-probobufreconstructor.c++
/// @brief Populate ProtoBuf message from Avro value
/// @author Tor Slettnes
//==============================================================================

#include "avro-protobufreconstructor.h++"
#include "avro-valuemethods.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"
#include "logging/logging.h++"

namespace cc::avro
{
    constexpr bool ENUMS_AS_STRINGS = false;

    bool ProtoBufReconstructor::reconstruct(
        const BaseValue& avro_value,
        ::google::protobuf::Message* msg)
    {
        if (avro_value.avro_type() == AVRO_RECORD)
        {
            return This::reconstruct_record(avro_value.avro_value(), msg);
        }
        else
        {
            return false;
        }
    }

    bool ProtoBufReconstructor::reconstruct_record(
        const avro_value_t& avro_value,
        ::google::protobuf::Message* msg)
    {
        const ::google::protobuf::Descriptor* descriptor = msg->GetDescriptor();
        const SchemaWrapper& wrapper = ProtoBufSchema::from_proto(descriptor);

        if (descriptor->well_known_type())
        {
            return This::reconstruct_wellknown(avro_value, msg);
        }
        else if (auto* value = dynamic_cast<cc::protobuf::variant::Value*>(msg))
        {
            return This::reconstruct_variant(avro_value, value);
        }
        else if (auto* list = dynamic_cast<cc::protobuf::variant::ValueList*>(msg))
        {
            return This::reconstruct_variant_list(avro_value, list);
        }
        else if (auto* map = dynamic_cast<cc::protobuf::variant::KeyValueMap*>(msg))
        {
            return This::reconstruct_variant_map(avro_value, map);
        }
        else
        {
            return This::reconstruct_custom(avro_value, msg);
        }
    }

    bool ProtoBufReconstructor::reconstruct_variant(
        const avro_value_t& avro_value,
        cc::protobuf::variant::Value* proto)
    {
        protobuf::encode(avro::get_value(avro_value, ENUMS_AS_STRINGS), proto);
        return true;
    }

    bool ProtoBufReconstructor::reconstruct_variant_list(
        const avro_value_t& avro_value,
        cc::protobuf::variant::ValueList* proto)
    {
        if (auto vlist = avro::get_array(avro_value, ENUMS_AS_STRINGS))
        {
            protobuf::encode(*vlist, proto);
            return true;
        }
        else
        {
            logf_notice(
                "Cannot reconstruct ProtoBuf ValueList from non-array Avro value %r",
                avro::get_value(avro_value, ENUMS_AS_STRINGS));
            return false;
        }
    }

    bool ProtoBufReconstructor::reconstruct_variant_map(
        const avro_value_t& avro_value,
        cc::protobuf::variant::KeyValueMap* proto)
    {
        if (auto kvmap = avro::get_map(avro_value, ENUMS_AS_STRINGS))
        {
            protobuf::encode(*kvmap, proto);
            return true;
        }
        else
        {
            logf_notice(
                "Cannot reconstruct ProtoBuf KeyValueMap from non-map Avro value %r",
                avro::get_value(avro_value, ENUMS_AS_STRINGS));;
            return false;
        }
    }

    bool ProtoBufReconstructor::reconstruct_wellknown(
        const avro_value_t& avro_value,
        ::google::protobuf::Message* msg)
    {
        bool reconstructed = false;

        switch (msg->GetDescriptor()->well_known_type())
        {
        case google::protobuf::Descriptor::WELLKNOWNTYPE_DOUBLEVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_double(avro_value),
                dynamic_cast<google::protobuf::DoubleValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_FLOATVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_float(avro_value),
                dynamic_cast<google::protobuf::FloatValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT64VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_long(avro_value),
                dynamic_cast<google::protobuf::UInt64Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT64VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_long(avro_value),
                dynamic_cast<google::protobuf::UInt64Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT32VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_int(avro_value),
                dynamic_cast<google::protobuf::Int32Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT32VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_int(avro_value),
                dynamic_cast<google::protobuf::Int32Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRINGVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_string(avro_value),
                dynamic_cast<google::protobuf::StringValue*>(msg));

            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BYTESVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_bytes(avro_value),
                dynamic_cast<google::protobuf::BytesValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BOOLVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_boolean(avro_value),
                dynamic_cast<google::protobuf::BoolValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_DURATION:
            reconstructed = protobuf::encode_optional(
                avro::get_duration(avro_value),
                dynamic_cast<google::protobuf::Duration*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP:
            reconstructed = protobuf::encode_optional(
                avro::get_timepoint(avro_value),
                dynamic_cast<google::protobuf::Timestamp*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_VALUE:
            protobuf::encode(
                avro::get_value(avro_value, ENUMS_AS_STRINGS),
                dynamic_cast<google::protobuf::Value*>(msg));
            reconstructed = true;
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_LISTVALUE:
            if (auto valuelist_ptr = avro::get_array(avro_value, ENUMS_AS_STRINGS))
            {
                protobuf::encode(
                    *valuelist_ptr,
                    dynamic_cast<google::protobuf::ListValue*>(msg));
                reconstructed = true;
            }
            else
            {
                reconstructed = false;
            }
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRUCT:
            if (auto kvmap_ptr = avro::get_map(avro_value, ENUMS_AS_STRINGS))
            {
                protobuf::encode(
                    *kvmap_ptr,
                    dynamic_cast<google::protobuf::Struct*>(msg));
                reconstructed = true;
            }
            else
            {
                reconstructed = false;
            }
            break;

        default:
            reconstructed = false;
        }

        if (!reconstructed)
        {
            logf_notice(
                "Cannot reconstruct ProtoBuf %s from Avro value type %s: %r",
                msg->GetDescriptor()->full_name(),
                avro::type_name(avro_value),
                avro::get_value(avro_value, ENUMS_AS_STRINGS));
        }
        return reconstructed;
    }

    bool ProtoBufReconstructor::reconstruct_custom(
        const avro_value_t& avro_value,
        ::google::protobuf::Message* msg)
    {
        const google::protobuf::Descriptor* descriptor = msg->GetDescriptor();
        int nfields = descriptor->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor* fd = descriptor->field(i);
            avro_value_t field_value = avro::get_field_by_name(
                avro_value,
                fd->name());
            This::reconstruct_field(field_value, msg, fd);
        }
        return true;
    }

    bool ProtoBufReconstructor::reconstruct_field(
        const avro_value_t& avro_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        if (fd->is_map())
        {
            return This::reconstruct_mapped_field(avro_value, msg, fd);
        }
        else if (fd->is_repeated())
        {
            return This::reconstruct_repeated_field(avro_value, msg, fd);
        }
        else if (fd->containing_oneof())
        {
            avro_value_t branch_value;
            if (avro_value_get_current_branch(
                    &avro_value,
                    &branch_value) == 0)
            {
                return This::reconstruct_single_field(branch_value, msg, fd);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return This::reconstruct_single_field(avro_value, msg, fd);
        }
    }

    bool ProtoBufReconstructor::reconstruct_single_field(
        const avro_value_t& avro_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        const google::protobuf::Reflection* reflection = msg->GetReflection();
        bool reconstructed = false;

        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            if (auto opt = avro::get_int(avro_value))
            {
                reflection->SetInt32(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            if (auto opt = avro::get_long(avro_value))
            {
                reflection->SetInt64(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            if (auto opt = avro::get_int(avro_value))
            {
                reflection->SetUInt32(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            if (auto opt = avro::get_long(avro_value))
            {
                reflection->SetUInt64(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            if (auto opt = avro::get_double(avro_value))
            {
                reflection->SetDouble(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            if (auto opt = avro::get_float(avro_value))
            {
                reflection->SetFloat(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            if (auto opt = avro::get_boolean(avro_value))
            {
                reflection->SetBool(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            if (auto opt = avro::get_enum_symbol(avro_value))
            {
                reconstructed = This::reconstruct_enum_field(avro_value, msg, fd);
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            if (auto opt = avro::get_string(avro_value))
            {
                reflection->SetString(msg, fd, *opt);
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            if (auto opt = avro::get_bytes(avro_value))
            {
                reflection->SetString(msg, fd, opt->as_string());
                reconstructed = true;
            }
            break;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            if (avro::type(avro_value) == AVRO_RECORD)
            {
                return This::reconstruct_record(
                    avro_value,
                    reflection->MutableMessage(msg, fd));
            }
            break;

        default:
            logf_warning(
                "Cannot reconstruct ProtoBuf field %s of type %s from Avro type %s",
                fd->name(),
                msg->GetDescriptor()->full_name(),
                avro::type_name(avro_value));
            break;
        }

        return false;
    }

    bool ProtoBufReconstructor::reconstruct_indexed_field(
        const avro_value_t& avro_value,
        google::protobuf::Message *msg,
        const google::protobuf::FieldDescriptor* fd,
        int index)
    {
        const google::protobuf::Reflection* reflection = msg->GetReflection();
        return false;
    }

    bool ProtoBufReconstructor::reconstruct_repeated_field(
        const avro_value_t& avro_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        return false;
    }

    bool ProtoBufReconstructor::reconstruct_mapped_field(
        const avro_value_t& avro_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        const google::protobuf::Descriptor* map_descriptor = fd->message_type();
        const google::protobuf::FieldDescriptor* fd_key = map_descriptor->map_key();
        const google::protobuf::FieldDescriptor* fd_value = map_descriptor->map_value();
        return false;
    }

    bool ProtoBufReconstructor::reconstruct_enum_field(
        const avro_value_t& avro_value,
        google::protobuf::Message* msg,
        const google::protobuf::FieldDescriptor* fd)
    {
        return false;
    }

}  // namespace cc::avro
