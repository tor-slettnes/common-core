/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message-decoder.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes
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
            return decoded_shared<core::types::ValueList>(
                *dynamic_cast<const google::protobuf::ListValue *>(&this->msg));

        case google::protobuf::Descriptor::WELLKNOWNTYPE_STRUCT:
            return decoded_shared<core::types::KeyValueMap>(
                *dynamic_cast<const google::protobuf::Struct *>(&this->msg));

        default:
            if (const auto *value = dynamic_cast<const cc::protobuf::variant::Value *>(&this->msg))
            {
                return decoded<core::types::Value>(*value);
            }
            else if (const auto *vlist = dynamic_cast<const cc::protobuf::variant::ValueList *>(&this->msg))
            {
                return decoded<core::types::ValueList>(*vlist);
            }
            else if (const auto *tvlist = dynamic_cast<const cc::protobuf::variant::TaggedValueList *>(&this->msg))
            {
                return decoded<core::types::TaggedValueList>(*tvlist);
            }
            else if (const auto *kvmap = dynamic_cast<const cc::protobuf::variant::KeyValueMap *>(&this->msg))
            {
                return decoded<core::types::KeyValueMap>(*kvmap);
            }
            else
            {
                return this->to_tvlist();
            }
        };
    }

    core::types::KeyValueMapPtr MessageDecoder::to_kvmap() const
    {
        return this->to_tvlist()->as_kvmap_ptr();
    }

    core::types::TaggedValueListPtr MessageDecoder::to_tvlist() const
    {
        int nfields = this->descriptor->field_count();
        auto tvlist = std::make_shared<core::types::TaggedValueList>();
        tvlist->reserve(nfields);

        for (int i = 0; i < nfields; i++)
        {
            const google::protobuf::FieldDescriptor *fd = this->descriptor->field(i);

            if (core::types::Value value = this->field_to_value(*fd))
            {
                tvlist->emplace_back(fd->name(), std::move(value));
            }
        }
        return tvlist;
    }

    core::types::Value MessageDecoder::field_to_value(
        const google::protobuf::FieldDescriptor &fd) const
    {
        core::types::Value result;
        if (fd.is_map())
        {
            if (this->reflection->FieldSize(this->msg, &fd) > 0)
            {
                result = this->mapped_field_to_tvlist(fd);
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

    core::types::TaggedValueListPtr MessageDecoder::mapped_field_to_tvlist(
        const google::protobuf::FieldDescriptor &fd) const
    {
        auto tvlist = std::make_shared<core::types::TaggedValueList>();
        int size = this->reflection->FieldSize(this->msg, &fd);
        tvlist->reserve(size);

        // A ProtoBuf map is really a repeated field comprising nested messages
        // of the form
        //
        //   ```protobuf
        //   message map_Entry
        //   {
        //       KeyType key = 1;
        //       ValueType value = 2;
        //   }
        //  ```
        //
        // We create a nested `MessageDecoder()` instance to access these as
        // a TaggedValueList.

        for (int n = 0; n < size; n++)
        {
            auto field_data = MessageDecoder(
                                  this->reflection->GetRepeatedMessage(this->msg, &fd, n),
                                  this->enums_as_strings)
                                  .to_tvlist();

            tvlist->emplace_back(
                field_data->front().as_string(),  // key
                field_data->back());              // value
        }
        return tvlist;
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
