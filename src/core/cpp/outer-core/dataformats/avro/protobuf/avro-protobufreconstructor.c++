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
    bool ProtoBufReconstructor::reconstruct(
        const BaseValue& avro_value,
        ::google::protobuf::Message* msg) const
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
        const BaseValue& avro_value,
        cc::protobuf::variant::Value* proto)
    {
        protobuf::encode(avro_value.as_value(), proto);
        return true;
    }

    bool ProtoBufReconstructor::reconstruct_variant_list(
        const BaseValue& avro_value,
        cc::protobuf::variant::ValueList* proto)
    {
        if (auto vlist = avro_value.as_value().get_valuelist_ptr())
        {
            protobuf::encode(*vlist, proto);
            return true;
        }
        else
        {
            logf_notice(
                "Cannot reconstruct ProtoBuf ValueList from non-array Avro value %r",
                avro_value.as_value());
            return false;
        }
    }

    bool ProtoBufReconstructor::reconstruct_variant_map(
        const BaseValue& avro_value,
        cc::protobuf::variant::KeyValueMap* proto)
    {
        if (auto kvmap = avro_value.as_value().get_kvmap_ptr())
        {
            protobuf::encode(*kvmap, proto);
            return true;
        }
        else
        {
            logf_notice(
                "Cannot reconstruct ProtoBuf KeyValueMap from non-map Avro value %r",
                avro_value.as_value());
            return false;
        }
    }

    bool ProtoBufReconstructor::reconstruct_wellknown(
        const BaseValue& avro_value,
        ::google::protobuf::Message* msg)
    {
        bool reconstructed = false;

        switch (msg->GetDescriptor()->well_known_type())
        {
        case google::protobuf::Descriptor::WELLKNOWNTYPE_DOUBLEVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_double(avro_value.avro_value()),
                dynamic_cast<google::protobuf::DoubleValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_FLOATVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_float(avro_value.avro_value()),
                dynamic_cast<google::protobuf::FloatValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT64VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_long(avro_value.avro_value()),
                dynamic_cast<google::protobuf::UInt64Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT64VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_long(avro_value.avro_value()),
                dynamic_cast<google::protobuf::UInt64Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT32VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_int(avro_value.avro_value()),
                dynamic_cast<google::protobuf::Int32Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT32VALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_int(avro_value.avro_value()),
                dynamic_cast<google::protobuf::Int32Value*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRINGVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_string(avro_value.avro_value()),
                dynamic_cast<google::protobuf::StringValue*>(msg));

            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BYTESVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_bytes(avro_value.avro_value()),
                dynamic_cast<google::protobuf::BytesValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BOOLVALUE:
            reconstructed = protobuf::encode_optional(
                avro::get_boolean(avro_value.avro_value()),
                dynamic_cast<google::protobuf::BoolValue*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_DURATION:
            reconstructed = protobuf::encode_optional(
                avro::get_duration(avro_value.avro_value()),
                dynamic_cast<google::protobuf::Duration*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP:
            reconstructed = protobuf::encode_optional(
                avro::get_timepoint(avro_value.avro_value()),
                dynamic_cast<google::protobuf::Timestamp*>(msg));
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_VALUE:
            protobuf::encode(
                avro_value.as_value(),
                dynamic_cast<google::protobuf::Value*>(msg));
            reconstructed = true;
            break;

        case google::protobuf::Descriptor::WELLKNOWNTYPE_LISTVALUE:
            if (auto valuelist_ptr = avro_value.as_value().get_valuelist_ptr())
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
            if (auto kvmap_ptr = avro_value.as_value().get_kvmap_ptr())
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
                avro_value.avro_type_name(),
                avro_value.as_value());
        }
        return reconstructed;
    }

    bool ProtoBufReconstructor::reconstruct_custom(
        const BaseValue& avro_value,
        ::google::protobuf::Message* msg)
    {
        const google::protobuf::Descriptor *descriptor = msg->GetDescriptor();
        int nfields = descriptor->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = descriptor->field(i);
            avro_value_t field_value = avro::get_field_by_name(
                avro_value.avro_value(),
                fd->name());
//            This::reconstruct_field(field_value, msg, fd);
        }
        return true;
    }


#if 0
    vool ProtoBufReconstructor::reconstruct_field(
        const avro_value_t &avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
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
            return true;
        }
        else
        {
            This::reconstruct_single_field(avro_value, msg, fd);
        }
    }

    void ProtoBufReconstructor::reconstruct_single_field(
        const avro_value_t &avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        const google::protobuf::Reflection *reflection = msg.GetReflection();
        switch (fd->type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            return true;

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            return true;

        default:
            logf_warning("No known Avro conversion for ProtoBuf message %s field %s",
                         msg.GetDescriptor()->full_name(),
                         fd->name());
            return false;;
        }
    }

    void ProtoBufReconstructor::reconstruct_indexed_field(
        const avro_value_t &avro_value,
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
            return true;

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            return true;

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
            This::reconstruct_enum_field(
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
            This::reconstruct_message(avro_value,
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

    void ProtoBufReconstructor::reconstruct_repeated_field(
        const avro_value_t &avro_value,
        const google::protobuf::Message &msg,
        const google::protobuf::FieldDescriptor *fd)
    {
        assertf(avro_value_get_type(avro_value) == AVRO_ARRAY,
                "Attempt to assign repeated ProtoBuf message %s value type %s to Avro non-array value type %s",
                msg.GetDescriptor()->full_name(),
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
            This::reconstruct_indexed_field(&element, msg, fd, n);
            // TRY THIS:
            // avro_value_reset(&element);
            // avro_value_decref(&element);
        }
    }

    void ProtoBufReconstructor::reconstruct_mapped_field(
        const avro_value_t &avro_value,
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
                "Attempt to assign ProtoBuf message %s map<%s, %s> to Avro non-map value type %s",
                msg.GetDescriptor()->full_name(),
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

            This::reconstruct_single_field(&element, item, fd_value);
        }
    }

    void ProtoBufReconstructor::reconstruct_enum_field(
        const avro_value_t &avro_value,
        const google::protobuf::EnumValueDescriptor *enum_value)
    {
        // avro::set_enum_value(avro_value,
        //                      enum_value->index(),
        //                      enum_value->number());
        avro::set_enum(avro_value, enum_value->index());
    }


#endif

}  // namespace cc::avro
