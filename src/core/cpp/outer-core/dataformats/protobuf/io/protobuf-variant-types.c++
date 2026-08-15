/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.c++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-variant-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Variant Value encoding

    void encode(const core::types::Value &value,
                variant::Value *msg) noexcept
    {
        switch (value.type())
        {
        case core::types::ValueType::NONE:
            break;

        case core::types::ValueType::BOOL:
            msg->set_value_bool(value.as_bool());
            break;

        case core::types::ValueType::UINT:
            msg->set_value_uint(value.as_uint64());
            break;

        case core::types::ValueType::SINT:
            msg->set_value_sint(value.as_sint64());
            break;

        case core::types::ValueType::REAL:
            msg->set_value_real(value.as_real());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            msg->set_value_string(value.get_string());
            break;

        case core::types::ValueType::BYTEVECTOR:
            msg->set_value_bytes(value.get_bytevector().to_string());
            break;

        case core::types::ValueType::TIMEPOINT:
            encode(value.get<core::dt::TimePoint>(), msg->mutable_value_timestamp());
            break;

        case core::types::ValueType::DURATION:
            encode(value.get<core::dt::Duration>(), msg->mutable_value_duration());
            break;

        case core::types::ValueType::VALUELIST:
            encode(value.get_valuelist(), msg->mutable_value_list());
            break;

        case core::types::ValueType::TVLIST:
            encode(value.get_tvlist(), msg->mutable_value_tvlist());
            break;

        case core::types::ValueType::KVMAP:
            encode(value.get_kvmap(), msg->mutable_value_kvmap());
            break;
        }
    }

    void decode(const variant::Value &msg,
                core::types::Value *value) noexcept
    {
        switch (msg.value_case())
        {
        case variant::Value::kValueBool:
            *value = msg.value_bool();
            break;

        case variant::Value::kValueUint:
            *value = msg.value_uint();
            break;

        case variant::Value::kValueSint:
            *value = msg.value_sint();
            break;

        case variant::Value::kValueReal:
            *value = msg.value_real();
            break;

        case variant::Value::kValueString:
            *value = msg.value_string();
            break;

        case variant::Value::kValueBytes:
            *value = ByteVector(msg.value_bytes().begin(), msg.value_bytes().end());
            break;

        case variant::Value::kValueTimestamp:
            *value = decoded<core::dt::TimePoint>(msg.value_timestamp());
            break;

        case variant::Value::kValueDuration:
            *value = decoded<core::dt::Duration>(msg.value_duration());
            break;

        case variant::Value::kValueList:
            *value = decoded_shared<core::types::ValueList>(msg.value_list());
            break;

        case variant::Value::kValueTvlist:
            *value = decoded_shared<core::types::TaggedValueList>(msg.value_tvlist());
            break;

        case variant::Value::kValueKvmap:
            *value = decoded_shared<core::types::KeyValueMap>(msg.value_kvmap());
            break;

        default:
            *value = std::monostate();
            break;
        }
    }

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const core::types::TaggedValue &tv,
                variant::TaggedValue *msg) noexcept
    {
        if (tv.first)
        {
            msg->set_tag(tv.first.value());
        }
        encode(tv.second, msg->mutable_value());
    }

    void decode(const variant::TaggedValue &msg,
                core::types::TaggedValue *tv) noexcept
    {
        if (msg.has_tag())
        {
            tv->first = msg.tag();
        }
        else
        {
            tv->first.reset();
        }
        decode(msg.value(), &tv->second);
    }

    void encode(const core::types::Tag &tag,
                const core::types::Value &value,
                variant::TaggedValue *msg) noexcept
    {
        if (tag.has_value())
        {
            msg->set_tag(tag.value());
        }
        encode(value, msg->mutable_value());
    }

    void decode(const variant::TaggedValue &msg,
                std::string *tag,
                core::types::Value *value) noexcept
    {
        *tag = msg.tag();
        decode(msg.value(), value);
    }

    void decode(const variant::TaggedValue &msg,
                std::optional<std::string> *tag,
                core::types::Value *value) noexcept
    {
        if (msg.has_tag())
        {
            *tag = msg.tag();
        }
        else
        {
            tag->reset();
        }

        decode(msg.value(), value);
    }

    //==========================================================================
    // TaggedValueList encoding/decoding

    void encode(const core::types::TaggedValueList &tvlist,
                variant::TaggedValueList *msg) noexcept
    {
        encode(tvlist, msg->mutable_items());
    }

    void decode(const variant::TaggedValueList &msg,
                core::types::TaggedValueList *tvlist) noexcept
    {
        decode(msg.items(), tvlist);
    }

    void encode(const core::types::TaggedValueList &tvlist,
                RepeatedTaggedValue *msgs) noexcept
    {
        msgs->Reserve(msgs->size() + tvlist.size());
        for (const auto &tv : tvlist)
        {
            encode(tv, msgs->Add());
        }
    }

    void decode(const RepeatedTaggedValue &msgs,
                core::types::TaggedValueList *tvlist) noexcept
    {
        tvlist->reserve(tvlist->size() + msgs.size());
        for (const auto &msg : msgs)
        {
            decode(msg, &tvlist->emplace_back());
        }
    }

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const core::types::KeyValueMap &map,
                variant::KeyValueMap *msg) noexcept
    {
        encode(map, msg->mutable_map());
    }

    void decode(const variant::KeyValueMap &msg,
                core::types::KeyValueMap *map) noexcept
    {
        decode(msg.map(), map);
    }

    void encode(const core::types::KeyValueMap &map,
                google::protobuf::Map<std::string, variant::Value> *msg) noexcept
    {
        for (const auto &[key, value] : map)
        {
            encode(value, &(*msg)[key]);
        }
    }

    void decode(const google::protobuf::Map<std::string, variant::Value> &msg,
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
                variant::ValueList *msg) noexcept
    {
        encode(list, msg->mutable_items());
    }

    void decode(const variant::ValueList &msg,
                core::types::ValueList *list) noexcept
    {
        decode(msg.items(), list);
    }

    void encode(const core::types::ValueList &list,
                RepeatedValue *msgs) noexcept
    {
        msgs->Reserve(msgs->size() + list.size());
        for (const core::types::Value &value : list)
        {
            encode(value, msgs->Add());
        }
    }

    void decode(const RepeatedValue &msgs,
                core::types::ValueList *list) noexcept
    {
        list->reserve(list->size() + msgs.size());
        for (const auto &msg : msgs)
        {
            decode(msg, &list->emplace_back());
        }
    }
}  // namespace cc::protobuf
