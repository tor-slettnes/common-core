/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message-decoder.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-message-decoder.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-enum.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"
#include "logging/logging.h++"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>

#include <iostream>

namespace protobuf
{
    MessageDecoder::MessageDecoder(const google::protobuf::Message &msg,
                                   bool enums_as_strings)
        : msg(msg),
          reflection(msg.GetReflection()),
          descriptor(msg.GetDescriptor()),
          enums_as_strings(enums_as_strings)
    {
    }

    core::types::Value MessageDecoder::to_value() const
    {
        switch (this->descriptor->well_known_type())
        {
        case google::protobuf::Descriptor::WELLKNOWNTYPE_DOUBLEVALUE:
            return dynamic_cast<const google::protobuf::DoubleValue *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_FLOATVALUE:
            return dynamic_cast<const google::protobuf::FloatValue *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT64VALUE:
            return dynamic_cast<const google::protobuf::Int64Value *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT64VALUE:
            return dynamic_cast<const google::protobuf::UInt64Value *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_INT32VALUE:
            return dynamic_cast<const google::protobuf::Int32Value *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_UINT32VALUE:
            return dynamic_cast<const google::protobuf::UInt32Value *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRINGVALUE:
            return dynamic_cast<const google::protobuf::StringValue *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BYTESVALUE:
            return core::types::ByteVector(
                dynamic_cast<const google::protobuf::BytesValue *>(&this->msg)->value());

        case google::protobuf::Descriptor::WELLKNOWNTYPE_BOOLVALUE:
            return dynamic_cast<const google::protobuf::BoolValue *>(&this->msg)->value();

        case google::protobuf::Descriptor::WELLKNOWNTYPE_DURATION:
            return decoded<core::dt::Duration>(
                *dynamic_cast<const google::protobuf::Duration *>(&this->msg));

        case google::protobuf::Descriptor::WELLKNOWNTYPE_TIMESTAMP:
            return decoded<core::dt::TimePoint>(
                *dynamic_cast<const google::protobuf::Timestamp *>(&this->msg));

        case google::protobuf::Descriptor::WELLKNOWNTYPE_VALUE:
            return decoded<core::types::Value>(
                *dynamic_cast<const google::protobuf::Value *>(&this->msg));

        case google::protobuf::Descriptor::WELLKNOWNTYPE_LISTVALUE:
            return decode_shared<core::types::ValueList>(
                *dynamic_cast<const google::protobuf::ListValue *>(&this->msg));

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRUCT:
            return decode_shared<core::types::KeyValueMap>(
                *dynamic_cast<const google::protobuf::Struct *>(&this->msg));

        default:
            if (const auto *value = dynamic_cast<const cc::variant::Value *>(&this->msg))
            {
                return decoded<core::types::Value>(*value);
            }
            else if (const auto *vlist = dynamic_cast<const cc::variant::ValueList *>(&this->msg))
            {
                return decoded<core::types::Value>(*vlist);
            }
            else
            {
                return this->to_kvmap();
            }
        };
    }

    core::types::KeyValueMapPtr MessageDecoder::to_kvmap() const
    {
        auto kvmap = std::make_shared<core::types::KeyValueMap>();
        int nfields = this->descriptor->field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = this->descriptor->field(i);

            if (core::types::Value value = this->field_to_value(*fd))
            {
                kvmap->insert_or_assign(fd->name(), std::move(value));
            }
        }
        return kvmap;
    }

    core::types::Value MessageDecoder::field_to_value(
        const google::protobuf::FieldDescriptor &fd) const
    {
        core::types::Value result;
        if (fd.is_map())
        {
            if (this->reflection->FieldSize(this->msg, &fd) > 0)
            {
                result = this->mapped_field_to_kvmap(fd);
            }
        }
        else if (fd.is_repeated())
        {
            if (this->reflection->FieldSize(this->msg, &fd) > 0)
            {
                result = this->repeated_field_to_valuelist(fd);
            }
        }
        else
        {
            if (this->reflection->HasField(this->msg, &fd) ||  // Value is present
                (fd.containing_oneof() == nullptr))            // Not optional
            {
                result = this->single_field_to_value(fd);
            }
        }
        return result;
    }

    core::types::Value MessageDecoder::single_field_to_value(
        const google::protobuf::FieldDescriptor &fd) const
    {
        switch (fd.type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            return this->reflection->GetInt32(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            return this->reflection->GetInt64(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            return this->reflection->GetUInt32(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            return this->reflection->GetUInt64(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            return this->reflection->GetDouble(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            return this->reflection->GetFloat(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            return this->reflection->GetBool(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            if (this->enums_as_strings)
            {
                return enum_name(
                    this->reflection->GetEnumValue(this->msg, &fd),
                    fd.enum_type());
            }
            else
            {
                return this->reflection->GetEnumValue(this->msg, &fd);
            }

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            return this->reflection->GetString(this->msg, &fd);

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            return core::types::ByteVector::from_string(
                this->reflection->GetString(this->msg, &fd));

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            return this->message_to_value(
                this->reflection->GetMessage(this->msg, &fd));

        default:
            return {};
        }
    }

    core::types::Value MessageDecoder::indexed_field_to_value(
        const google::protobuf::FieldDescriptor &fd,
        int repeat_index) const
    {
        switch (fd.type())
        {
        case google::protobuf::FieldDescriptor::TYPE_INT32:
        case google::protobuf::FieldDescriptor::TYPE_SINT32:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED32:
            return this->reflection->GetRepeatedInt32(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_INT64:
        case google::protobuf::FieldDescriptor::TYPE_SINT64:
        case google::protobuf::FieldDescriptor::TYPE_SFIXED64:
            return this->reflection->GetRepeatedInt64(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_UINT32:
        case google::protobuf::FieldDescriptor::TYPE_FIXED32:
            return this->reflection->GetRepeatedUInt32(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_UINT64:
        case google::protobuf::FieldDescriptor::TYPE_FIXED64:
            return this->reflection->GetRepeatedUInt64(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
            return this->reflection->GetRepeatedDouble(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_FLOAT:
            return this->reflection->GetRepeatedFloat(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_BOOL:
            return this->reflection->GetRepeatedBool(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_ENUM:
            if (this->enums_as_strings)
            {
                return enum_name(
                    this->reflection->GetRepeatedEnumValue(this->msg, &fd, repeat_index),
                    fd.enum_type());
            }
            else
            {
                return this->reflection->GetRepeatedEnumValue(this->msg, &fd, repeat_index);
            }

        case google::protobuf::FieldDescriptor::TYPE_STRING:
            return this->reflection->GetRepeatedString(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::TYPE_BYTES:
            return core::types::ByteVector::from_string(
                this->reflection->GetRepeatedString(this->msg, &fd, repeat_index));

        case google::protobuf::FieldDescriptor::TYPE_MESSAGE:
        case google::protobuf::FieldDescriptor::TYPE_GROUP:
            return this->message_to_value(
                this->reflection->GetRepeatedMessage(this->msg, &fd, repeat_index));

        default:
            return {};
        }
    }

    core::types::ValueListPtr MessageDecoder::repeated_field_to_valuelist(
        const google::protobuf::FieldDescriptor &fd) const
    {
        auto vlist = std::make_shared<core::types::ValueList>();
        int size = this->reflection->FieldSize(this->msg, &fd);
        vlist->reserve(size);

        for (int n = 0; n < size; n++)
        {
            vlist->push_back(this->indexed_field_to_value(fd, n));
        }
        return vlist;
    }

    core::types::KeyValueMapPtr MessageDecoder::mapped_field_to_kvmap(
        const google::protobuf::FieldDescriptor &fd) const
    {
        auto kvmap = std::make_shared<core::types::KeyValueMap>();
        int size = this->reflection->FieldSize(this->msg, &fd);

        for (int n = 0; n < size; n++)
        {
            auto field_map = MessageDecoder(
                                 this->reflection->GetRepeatedMessage(this->msg, &fd, n),
                                 this->enums_as_strings)
                                 .to_kvmap();

            kvmap->insert_or_assign(
                field_map->get("key").as_string(),
                field_map->get("value"));
        }
        return kvmap;
    }

    core::types::Value MessageDecoder::message_to_value(
        const google::protobuf::Message &msg) const
    {
        return MessageDecoder(msg, this->enums_as_strings).to_value();
    }

    //--------------------------------------------------------------------------
    // to_value() method

    core::types::Value to_value(const google::protobuf::Message &msg,
                                bool enums_as_strings)
    {
        return MessageDecoder(msg, enums_as_strings).to_value();
    }
}  // namespace protobuf

/// Additional convenience operators for ProtoBuf messages
namespace google::protobuf
{
    std::ostream &operator<<(std::ostream &stream, const Message &msg)
    {
        return stream << ::protobuf::to_value(msg);
    }
}  // namespace google::protobuf
