/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Variant Value encoding

    void encode(const cc::types::Value &value,
                CC::Variant::Value *msg) noexcept
    {
        switch (value.type())
        {
        case types::ValueType::NONE:
            break;

        case types::ValueType::BOOL:
            msg->set_value_bool(value.as_bool());
            break;

        case types::ValueType::UINT:
            msg->set_value_uint(value.as_largest_uint());
            break;

        case types::ValueType::SINT:
            msg->set_value_sint(value.as_largest_sint());
            break;

        case types::ValueType::REAL:
            msg->set_value_real(value.as_real());
            break;

        case types::ValueType::CHAR:
        case types::ValueType::STRING:
            msg->set_value_string(value.as_string());
            break;

        case types::ValueType::BYTEARRAY:
            msg->set_value_bytes(value.as_bytearray().to_string());
            break;

        case types::ValueType::TIMEPOINT:
            encode(value.as_timepoint(), msg->mutable_value_timestamp());
            break;

        case types::ValueType::DURATION:
            encode(value.as_duration(), msg->mutable_value_duration());
            break;

        case types::ValueType::VALUELIST:
            encode(value.as_valuelist(), msg->mutable_value_list());
            break;

        case types::ValueType::KVMAP:
            encode(value.as_kvmap(), msg->mutable_value_list());
            break;

        case types::ValueType::TVLIST:
            encode(value.as_tvlist(), msg->mutable_value_list());
            break;
        }
    }

    void decode(const CC::Variant::Value &msg,
                cc::types::Value *value) noexcept
    {
        switch (msg.value_case())
        {
        case CC::Variant::Value::kValueBool:
            *value = msg.value_bool();
            break;

        case CC::Variant::Value::kValueUint:
            *value = msg.value_uint();
            break;

        case CC::Variant::Value::kValueSint:
            *value = msg.value_sint();
            break;

        case CC::Variant::Value::kValueReal:
            *value = msg.value_real();
            break;

        case CC::Variant::Value::kValueString:
            *value = msg.value_string();
            break;

        case CC::Variant::Value::kValueBytes:
            *value = ByteArray(msg.value_bytes().begin(), msg.value_bytes().end());
            break;

        case CC::Variant::Value::kValueTimestamp:
            *value = protobuf::decoded<dt::TimePoint>(msg.value_timestamp());
            break;

        case CC::Variant::Value::kValueDuration:
            *value = protobuf::decoded<dt::Duration>(msg.value_duration());
            break;

        case CC::Variant::Value::kValueList:
            decode(msg.value_list(), value);
            break;

        default:
            *value = std::monostate();
            break;
        }
    }

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const cc::types::TaggedValue &tv,
                CC::Variant::Value *msg) noexcept
    {
        if (tv.first.has_value())
        {
            msg->set_tag(tv.first.value());
        }
        if (!tv.second.empty())
        {
            encode(tv.second, msg);
        }
    }

    void decode(const CC::Variant::Value &msg,
                cc::types::TaggedValue *tv) noexcept
    {
        if (msg.tag().length() > 0)
        {
            tv->first = msg.tag();
        }
        decode(msg, &tv->second);
    }

    void encode(const cc::types::Tag &tag,
                const cc::types::Value &value,
                CC::Variant::Value *msg) noexcept
    {
        msg->set_tag(tag.value_or(""));
        encode(value, msg);
    }

    void decode(const CC::Variant::Value &msg,
                std::string *tag,
                cc::types::Value *value) noexcept
    {
        *tag = msg.tag();
        decode(msg, value);
    }

    //==========================================================================
    // TaggedValueList encoding/decoding

    void encode(const cc::types::TaggedValueList &tvlist,
                CC::Variant::ValueList *msg) noexcept
    {
        encode(tvlist, msg->mutable_items());
        msg->set_untagged(!tvlist.tagged());
        msg->set_mappable(tvlist.mappable());
    }

    void decode(const CC::Variant::ValueList &msg,
                cc::types::TaggedValueList *tvlist) noexcept
    {
        decode(msg.items(), tvlist);
    }

    void encode(const cc::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept
    {
        msg->Reserve(tvlist.size());
        for (const auto &tv : tvlist)
        {
            encode(tv, msg->Add());
        }
    }

    void decode(const RepeatedValue &msgs,
                cc::types::TaggedValueList *tvlist) noexcept
    {
        tvlist->reserve(tvlist->size() + msgs.size());
        for (const auto &msg : msgs)
        {
            decode(msg, &tvlist->emplace_back());
        }
    }

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const cc::types::KeyValueMap &map,
                CC::Variant::ValueList *msg) noexcept
    {
        msg->mutable_items()->Reserve(msg->items_size() + map.size());
        for (const auto &[key, value] : map)
        {
            CC::Variant::Value *tv = msg->add_items();
            tv->set_tag(key);
            encode(value, tv);
        }
        msg->set_mappable(true);
    }

    void decode(const CC::Variant::ValueList &msg,
                cc::types::KeyValueMap *map) noexcept
    {
        decode(msg.items(), map);
    }

    void encode(const cc::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept
    {
        msg->Reserve(map.size());
        for (const auto &item : map)
        {
            encode(item, msg->Add());
        }
    }

    void decode(const RepeatedValue &msgs,
                cc::types::KeyValueMap *kvmap) noexcept
    {
        for (const auto &msg : msgs)
        {
            if (!msg.tag().empty())
            {
                decode(msg, &(*kvmap)[msg.tag()]);
            }
        }
    }

    void encode(const cc::types::KeyValueMap &map,
                google::protobuf::Map<std::string, CC::Variant::Value> *msg) noexcept
    {
        for (const auto &[key, value] : map)
        {
            encode(value, &(*msg)[key]);
        }
    }

    void decode(const google::protobuf::Map<std::string, CC::Variant::Value> &msg,
                cc::types::KeyValueMap *map) noexcept
    {
        for (const auto &[key, value] : msg)
        {
            decode(value, &(*map)[key]);
        }
    }

    //==========================================================================
    // ValueList encoding/decoding

    void encode(const cc::types::ValueList &list,
                CC::Variant::ValueList *msg) noexcept
    {
        msg->mutable_items()->Reserve(msg->items_size() + list.size());
        for (const types::Value &value : list)
        {
            encode(value, msg->add_items());
        }
        msg->set_untagged(true);
    }

    void decode(const CC::Variant::ValueList &msg,
                cc::types::ValueList *list) noexcept
    {
        decode(msg.items(), list);
    }

    void decode(const RepeatedValue &msgs,
                cc::types::ValueList *list) noexcept
    {
        list->reserve(list->size() + msgs.size());
        for (const auto &msg : msgs)
        {
            decode(msg, &list->emplace_back());
        }
    }

    void decode(const CC::Variant::ValueList &msg,
                cc::types::Value *value) noexcept
    {
        if (msg.untagged())
        {
            // The sender has indicated that tags can be ignored, so we'll store
            // this as a plain ValueList instance
            *value = decoded<types::ValueList>(msg);
        }
        else if (msg.mappable())
        {
            // The sender has indicated that tags are unique, so we'll store
            // this as a KeyValueMap instance.
            *value = decoded<types::KeyValueMap>(msg);
        }
        else
        {
            // The sender did not indicate that tags can be ignored, nor that
            // they are unique. We start by assuming we'll need a TaggedValueList.
            auto tvlist = decoded<types::TaggedValueList>(msg);

            // Let's see if we can ignore tags and just store just the values
            if (!tvlist.tagged())
            {
                *value = tvlist.values();
            }

            // Nope, tags are present. Are they unique?
            else if (tvlist.mappable())
            {
                *value = tvlist.as_kvmap();
            }

            // Nope.  Let's stick to a TaggedValueList container.
            else
            {
                *value = tvlist;
            }
        }
    }

}  // namespace cc::protobuf
