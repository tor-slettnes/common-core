/// -*- c++ -*-
//==============================================================================
/// @file protobuf-standard-types.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

#include <utility>

namespace protobuf
{
    void encode(const std::string &s,
                google::protobuf::StringValue *msg) noexcept
    {
        msg->set_value(s);
    }

    void decode(const google::protobuf::StringValue &msg,
                std::string *s) noexcept
    {
        *s = msg.value();
    }

    void encode(double r,
                google::protobuf::DoubleValue *msg) noexcept
    {
        msg->set_value(r);
    }

    void decode(const google::protobuf::DoubleValue &msg,
                double *r) noexcept
    {
        *r = msg.value();
    }

    void encode(float r,
                google::protobuf::FloatValue *msg) noexcept
    {
        msg->set_value(r);
    }

    void decode(const google::protobuf::FloatValue &msg,
                float *r) noexcept
    {
        *r = msg.value();
    }

    void encode(std::uint64_t n,
                google::protobuf::UInt64Value *msg) noexcept
    {
        msg->set_value(n);
    }

    void decode(const google::protobuf::UInt64Value &msg,
                std::uint64_t *n) noexcept
    {
        *n = msg.value();
    }

    void encode(std::int64_t n,
                google::protobuf::Int64Value *msg) noexcept
    {
        msg->set_value(n);
    }

    void decode(const google::protobuf::Int64Value &msg,
                std::int64_t *n) noexcept
    {
        *n = msg.value();
    }

    void encode(std::uint32_t n,
                google::protobuf::UInt32Value *msg) noexcept
    {
        msg->set_value(n);
    }

    void decode(const google::protobuf::UInt32Value &msg,
                std::uint32_t *n) noexcept
    {
        *n = msg.value();
    }

    void encode(std::int32_t n,
                google::protobuf::Int32Value *msg) noexcept
    {
        msg->set_value(n);
    }

    void decode(const google::protobuf::Int32Value &msg,
                std::int32_t *n) noexcept
    {
        *n = msg.value();
    }

    void encode(bool b,
                google::protobuf::BoolValue *msg) noexcept
    {
        msg->set_value(b);
    }

    void decode(const google::protobuf::BoolValue &msg,
                bool *b) noexcept
    {
        *b = msg.value();
    }

    void encode(const core::types::Bytes &b,
                google::protobuf::BytesValue *msg) noexcept
    {
        msg->set_value(b.data(), b.size());
    }

    void decode(const google::protobuf::BytesValue &msg,
                core::types::Bytes *b) noexcept
    {
        b->assign(msg.value().begin(), msg.value().end());
    }

    void encode(const core::dt::TimePoint &tp,
                google::protobuf::Timestamp *ts) noexcept
    {
        timespec tspec = core::dt::to_timespec(tp);
        ts->set_seconds(tspec.tv_sec);
        ts->set_nanos((int)tspec.tv_nsec);
    }

    void decode(const google::protobuf::Timestamp &ts,
                core::dt::TimePoint *tp) noexcept
    {
        *tp = core::dt::to_timepoint(ts.seconds(), ts.nanos());
    }

    void encode(const core::dt::Duration &duration,
                google::protobuf::Duration *msg) noexcept
    {
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(duration);
        if (secs > duration)
            secs -= std::chrono::seconds(1);
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(duration - secs);
        msg->set_seconds(secs.count());
        msg->set_nanos(nanos.count());
    }

    void decode(const google::protobuf::Duration &msg,
                core::dt::Duration *duration) noexcept
    {
        *duration = (std::chrono::seconds(msg.seconds()) +
                     std::chrono::nanoseconds(msg.nanos()));
    }

    void encode(const core::types::Value &value,
                google::protobuf::Value *msg) noexcept
    {
        switch (value.type())
        {
        case core::types::ValueType::NONE:
            msg->set_null_value({});
            break;

        case core::types::ValueType::BOOL:
            msg->set_bool_value(value.as_bool());
            break;

        case core::types::ValueType::UINT:
        case core::types::ValueType::SINT:
        case core::types::ValueType::REAL:
        case core::types::ValueType::TIMEPOINT:
        case core::types::ValueType::DURATION:
        case core::types::ValueType::COMPLEX:
            msg->set_number_value(value.as_real());
            break;

        case core::types::ValueType::CHAR:
        case core::types::ValueType::STRING:
            msg->set_string_value(value.as_string());
            break;

        case core::types::ValueType::BYTEVECTOR:
            msg->set_string_value(value.as_bytevector().to_string());
            break;

        case core::types::ValueType::VALUELIST:
            encode(*value.get_valuelist(), msg->mutable_list_value());
            break;

        case core::types::ValueType::KVMAP:
            encode(*value.get_kvmap(), msg->mutable_struct_value());
            break;

        case core::types::ValueType::TVLIST:
            encode(*value.get_tvlist(), msg->mutable_struct_value());
            break;

        default:
            break;
        }
    }

    void decode(const google::protobuf::Value &msg,
                core::types::Value *value) noexcept
    {
        switch (msg.kind_case())
        {
        case google::protobuf::Value::kNullValue:
            *value = {};
            break;

        case google::protobuf::Value::kNumberValue:
            *value = msg.number_value();
            break;

        case google::protobuf::Value::kStringValue:
            *value = msg.string_value();
            break;

        case google::protobuf::Value::kBoolValue:
            *value = msg.bool_value();
            break;

        case google::protobuf::Value::kStructValue:
            *value = protobuf::decoded_shared<core::types::KeyValueMap>(msg.struct_value());
            break;

        case google::protobuf::Value::kListValue:
            *value = protobuf::decoded_shared<core::types::ValueList>(msg.list_value());
            break;

        default:
            break;
        }
    }

    void encode(const core::types::KeyValueMap &kvmap,
                google::protobuf::Struct *msg) noexcept
    {
        auto fields = msg->mutable_fields();
        for (const auto &[key, value] : kvmap)
        {
            encode(value, &(*fields)[key]);
        }
    }

    void decode(const google::protobuf::Struct &msg,
                core::types::KeyValueMap *kvmap) noexcept
    {
        for (const auto &[key, value] : msg.fields())
        {
            decode(value, &(*kvmap)[key]);
        }
    }

    void encode(const core::types::TaggedValueList &tvlist,
                google::protobuf::Struct *msg) noexcept
    {
        auto fields = msg->mutable_fields();
        for (const auto &[key, value] : tvlist)
        {
            if (key)
            {
                encode(value, &(*fields)[*key]);
            }
        }
    }

    void decode(const google::protobuf::Struct &msg,
                core::types::TaggedValueList *tvlist) noexcept
    {
        const auto &fields = msg.fields();
        for (const auto &[key, value] : fields)
        {
            core::types::TaggedValue tv = {key, {}};
            decode(value, &tv.second);
            tvlist->push_back(std::move(tv));
        }
    }

    void encode(const core::types::ValueList &list,
                google::protobuf::ListValue *msg) noexcept
    {
        for (const core::types::Value &value : list)
        {
            encode(value, msg->add_values());
        }
    }

    void decode(const google::protobuf::ListValue &msg,
                core::types::ValueList *list) noexcept
    {
        for (const google::protobuf::Value &value : msg.values())
        {
            decode(value, &list->emplace_back());
        }
    }
}  // namespace protobuf
