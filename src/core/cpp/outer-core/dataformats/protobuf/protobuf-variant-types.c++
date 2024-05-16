/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace core::io::proto
{
    //==========================================================================
    // Variant Value encoding

    void encode(const core::types::Value &value,
                cc::variant::Value *msg) noexcept
    {
        switch (value.type())
        {
        case core::types::ValueType::NONE:
            break;

        case core::types::ValueType::BOOL:
            msg->set_value_bool(value.as_bool());
            break;

        case core::types::ValueType::UINT:
            msg->set_value_uint(value.as_largest_uint());
            break;

        case core::types::ValueType::SINT:
            msg->set_value_sint(value.as_largest_sint());
            break;

        case core::types::ValueType::REAL:
            msg->set_value_real(value.as_real());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            msg->set_value_string(value.as_string());
            break;

        case core::types::ValueType::BYTEVECTOR:
            msg->set_value_bytes(value.as_bytevector().to_string());
            break;

        case core::types::ValueType::TIMEPOINT:
            encode(value.as_timepoint(), msg->mutable_value_timestamp());
            break;

        case core::types::ValueType::DURATION:
            encode(value.as_duration(), msg->mutable_value_duration());
            break;

        case core::types::ValueType::VALUELIST:
            encode(value.as_valuelist(), msg->mutable_value_list());
            break;

        case core::types::ValueType::KVMAP:
            encode(value.as_kvmap(), msg->mutable_value_list());
            break;

        case core::types::ValueType::TVLIST:
            encode(value.as_tvlist(), msg->mutable_value_list());
            break;
        }
    }

    void decode(const cc::variant::Value &msg,
                core::types::Value *value) noexcept
    {
        switch (msg.value_case())
        {
        case cc::variant::Value::kValueBool:
            *value = msg.value_bool();
            break;

        case cc::variant::Value::kValueUint:
            *value = msg.value_uint();
            break;

        case cc::variant::Value::kValueSint:
            *value = msg.value_sint();
            break;

        case cc::variant::Value::kValueReal:
            *value = msg.value_real();
            break;

        case cc::variant::Value::kValueString:
            *value = msg.value_string();
            break;

        case cc::variant::Value::kValueBytes:
            *value = ByteVector(msg.value_bytes().begin(), msg.value_bytes().end());
            break;

        case cc::variant::Value::kValueTimestamp:
            *value = ::core::io::proto::decoded<core::dt::TimePoint>(msg.value_timestamp());
            break;

        case cc::variant::Value::kValueDuration:
            *value = ::core::io::proto::decoded<core::dt::Duration>(msg.value_duration());
            break;

        case cc::variant::Value::kValueList:
            decode(msg.value_list(), value);
            break;

        default:
            *value = std::monostate();
            break;
        }
    }

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const core::types::TaggedValue &tv,
                cc::variant::Value *msg) noexcept
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

    void decode(const cc::variant::Value &msg,
                core::types::TaggedValue *tv) noexcept
    {
        if (msg.tag().length() > 0)
        {
            tv->first = msg.tag();
        }
        decode(msg, &tv->second);
    }

    void encode(const core::types::Tag &tag,
                const core::types::Value &value,
                cc::variant::Value *msg) noexcept
    {
        msg->set_tag(tag.value_or(""));
        encode(value, msg);
    }

    void decode(const cc::variant::Value &msg,
                std::string *tag,
                core::types::Value *value) noexcept
    {
        *tag = msg.tag();
        decode(msg, value);
    }

    //==========================================================================
    // TaggedValueList encoding/decoding

    void encode(const core::types::TaggedValueList &tvlist,
                cc::variant::ValueList *msg) noexcept
    {
        encode(tvlist, msg->mutable_items());
        msg->set_untagged(!tvlist.tagged());
        msg->set_mappable(tvlist.mappable());
    }

    void decode(const cc::variant::ValueList &msg,
                core::types::TaggedValueList *tvlist) noexcept
    {
        decode(msg.items(), tvlist);
    }

    void encode(const core::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept
    {
        msg->Reserve(tvlist.size());
        for (const auto &tv : tvlist)
        {
            encode(tv, msg->Add());
        }
    }

    void decode(const RepeatedValue &msgs,
                core::types::TaggedValueList *tvlist) noexcept
    {
        for (const auto &msg : msgs)
        {
            decode(msg, &tvlist->emplace_back());
        }
    }

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const core::types::KeyValueMap &map,
                cc::variant::ValueList *msg) noexcept
    {
        msg->mutable_items()->Reserve(msg->items_size() + map.size());
        for (const auto &[key, value] : map)
        {
            cc::variant::Value *tv = msg->add_items();
            tv->set_tag(key);
            encode(value, tv);
        }
        msg->set_mappable(true);
    }

    void decode(const cc::variant::ValueList &msg,
                core::types::KeyValueMap *map) noexcept
    {
        decode(msg.items(), map);
    }

    void encode(const core::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept
    {
        msg->Reserve(map.size());
        for (const auto &item : map)
        {
            encode(item, msg->Add());
        }
    }

    void decode(const RepeatedValue &msgs,
                core::types::KeyValueMap *kvmap) noexcept
    {
        for (const auto &msg : msgs)
        {
            if (!msg.tag().empty())
            {
                decode(msg, &(*kvmap)[msg.tag()]);
            }
        }
    }

    void encode(const core::types::KeyValueMap &map,
                google::protobuf::Map<std::string, cc::variant::Value> *msg) noexcept
    {
        for (const auto &[key, value] : map)
        {
            encode(value, &(*msg)[key]);
        }
    }

    void decode(const google::protobuf::Map<std::string, cc::variant::Value> &msg,
                core::types::KeyValueMap *map) noexcept
    {
        for (const auto &[key, value] : msg)
        {
            decode(value, &(*map)[key]);
        }
    }

    //==========================================================================
    // ValueList encoding/decoding

    void encode(const core::types::ValueList &list,
                cc::variant::ValueList *msg) noexcept
    {
        msg->mutable_items()->Reserve(msg->items_size() + list.size());
        for (const core::types::Value &value : list)
        {
            encode(value, msg->add_items());
        }
        msg->set_untagged(true);
    }

    void decode(const cc::variant::ValueList &msg,
                core::types::ValueList *list) noexcept
    {
        decode(msg.items(), list);
    }

    void decode(const RepeatedValue &msgs,
                core::types::ValueList *list) noexcept
    {
        for (const auto &msg : msgs)
        {
            decode(msg, &list->emplace_back());
        }
    }

    void decode(const cc::variant::ValueList &msg,
                core::types::Value *value) noexcept
    {
        if (msg.untagged())
        {
            // The sender has indicated that tags can be ignored, so we'll store
            // this as a plain ValueList instance
            *value = decoded<core::types::ValueList>(msg);
        }
        else if (msg.mappable())
        {
            // The sender has indicated that tags are unique, so we'll store
            // this as a KeyValueMap instance.
            *value = decoded<core::types::KeyValueMap>(msg);
        }
        else
        {
            // The sender did not indicate that tags can be ignored, nor that
            // they are unique. We start by assuming we'll need a TaggedValueList.
            auto tvlist = decoded<core::types::TaggedValueList>(msg);

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

}  // namespace core::io::proto
