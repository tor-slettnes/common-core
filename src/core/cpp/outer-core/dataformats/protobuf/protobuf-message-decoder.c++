/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-message-decoder.h++"
#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
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
          ref(*msg.GetReflection()),
          descriptor(*msg.GetDescriptor()),
          enums_as_strings(enums_as_strings)
    {
    }

    core::types::Value MessageDecoder::to_value() const
    {
        if (const auto *value = dynamic_cast<const cc::variant::Value *>(&this->msg))
        {
            return decoded<core::types::Value>(*value);
        }
        else if (const auto *vlist = dynamic_cast<const cc::variant::ValueList *>(&this->msg))
        {
            return decoded<core::types::Value>(*vlist);
        }
        else if (const auto *timestamp = dynamic_cast<const google::protobuf::Timestamp *>(&this->msg))
        {
            return decoded<core::dt::TimePoint>(*timestamp);
        }
        else if (const auto *duration = dynamic_cast<const google::protobuf::Duration *>(&this->msg))
        {
            return decoded<core::dt::Duration>(*duration);
        }
        else
        {
            return this->to_kvmap();
        }
    }

    core::types::KeyValueMapPtr MessageDecoder::to_kvmap() const
    {
        auto kvmap = std::make_shared<core::types::KeyValueMap>();
        int nfields = this->descriptor.field_count();
        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = this->descriptor.field(i);

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
            if (this->ref.FieldSize(this->msg, &fd) > 0)
            {
                result = this->mapped_field_to_kvmap(fd);
            }
        }
        else if (fd.is_repeated())
        {
            if (this->ref.FieldSize(this->msg, &fd) > 0)
            {
                result = this->repeated_field_to_valuelist(fd);
            }
        }
        else
        {
            if (this->ref.HasField(this->msg, &fd) ||
                (fd.containing_oneof() == nullptr))
            {
                result = this->single_field_to_value(fd);
            }
        }
        return result;
    }

    core::types::Value MessageDecoder::single_field_to_value(
        const google::protobuf::FieldDescriptor &fd) const
    {
        switch (fd.cpp_type())
        {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            return this->ref.GetInt32(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            return this->ref.GetInt64(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            return this->ref.GetUInt32(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            return this->ref.GetUInt64(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            return this->ref.GetDouble(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            return this->ref.GetFloat(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            return this->ref.GetBool(this->msg, &fd);

        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            if (this->enums_as_strings)
            {
                return this->ref.GetEnum(this->msg, &fd)->name();
            }
            else
            {
                return this->ref.GetEnumValue(this->msg, &fd);
            }

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            if (fd.type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
            {
                return core::types::ByteVector::from_string(this->ref.GetString(this->msg, &fd));
            }
            else
            {
                return this->ref.GetString(this->msg, &fd);
            }

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            return this->message_to_value(this->ref.GetMessage(this->msg, &fd));

        default:
            return {};
        }
    }

    core::types::Value MessageDecoder::indexed_field_to_value(
        const google::protobuf::FieldDescriptor &fd,
        int repeat_index) const
    {
        switch (fd.cpp_type())
        {
        case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
            return this->ref.GetRepeatedInt32(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
            return this->ref.GetRepeatedInt64(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
            return this->ref.GetRepeatedUInt32(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
            return this->ref.GetRepeatedUInt64(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
            return this->ref.GetRepeatedDouble(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
            return this->ref.GetRepeatedFloat(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
            return this->ref.GetRepeatedBool(this->msg, &fd, repeat_index);

        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            if (this->enums_as_strings)
            {
                return this->ref.GetRepeatedEnum(this->msg, &fd, repeat_index)->name();
            }
            else
            {
                return this->ref.GetRepeatedEnumValue(this->msg, &fd, repeat_index);
            }

        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            if (fd.type() == google::protobuf::FieldDescriptor::TYPE_BYTES)
            {
                return core::types::ByteVector::from_string(
                    this->ref.GetRepeatedString(this->msg, &fd, repeat_index));
            }
            else
            {
                return this->ref.GetRepeatedString(this->msg, &fd, repeat_index);
            }

        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            return this->message_to_value(
                this->ref.GetRepeatedMessage(this->msg, &fd, repeat_index));

        default:
            return {};
        }
    }

    core::types::ValueListPtr MessageDecoder::repeated_field_to_valuelist(
        const google::protobuf::FieldDescriptor &fd) const
    {
        auto vlist = std::make_shared<core::types::ValueList>();
        int size = this->ref.FieldSize(this->msg, &fd);
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
        int size = this->ref.FieldSize(this->msg, &fd);

        for (int n = 0; n < size; n++)
        {
            auto field_map = MessageDecoder(
                                 this->ref.GetRepeatedMessage(this->msg, &fd, n),
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
