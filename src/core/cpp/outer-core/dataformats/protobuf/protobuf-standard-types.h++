/// -*- c++ -*-
//==============================================================================
/// @file protobuf-standard-types.h++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "chrono/date-time.h++"
#include "types/value.h++"

#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/timestamp.pb.h>
#include <google/protobuf/duration.pb.h>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/wrappers.pb.h>
#include <google/protobuf/struct.pb.h>

#include <google/protobuf/util/time_util.h>  // ProtoBuf Duration operators

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
{
    using google::protobuf::BoolValue;
    using google::protobuf::BytesValue;
    using google::protobuf::DoubleValue;
    using google::protobuf::Duration;
    using google::protobuf::Empty;
    using google::protobuf::FloatValue;
    using google::protobuf::Int32Value;
    using google::protobuf::Int64Value;
    using google::protobuf::ListValue;
    using google::protobuf::Map;
    using google::protobuf::Message;
    using google::protobuf::StringValue;
    using google::protobuf::Struct;
    using google::protobuf::Timestamp;
    using google::protobuf::UInt32Value;
    using google::protobuf::UInt64Value;
    using google::protobuf::Value;

    using IntValue = google::protobuf::Int64Value;
    using UIntValue = google::protobuf::UInt64Value;
    using RealValue = google::protobuf::DoubleValue;
    using RepeatedString = google::protobuf::RepeatedPtrField<std::string>;


    //==========================================================================
    // Well-known ProtoBuf wrapper types

    void encode(const std::string &s,
                google::protobuf::StringValue *msg) noexcept;
    void decode(const google::protobuf::StringValue &msg,
                std::string *s) noexcept;

    void encode(double r,
                google::protobuf::DoubleValue *msg) noexcept;

    void decode(const google::protobuf::DoubleValue &msg,
                double *r) noexcept;

    void encode(float r,
                google::protobuf::FloatValue *msg) noexcept;

    void decode(const google::protobuf::FloatValue &msg,
                float *r) noexcept;

    void encode(std::uint64_t n,
                google::protobuf::UInt64Value *msg) noexcept;

    void decode(const google::protobuf::UInt64Value &msg,
                std::uint64_t *n) noexcept;

    void encode(std::int64_t n,
                google::protobuf::Int64Value *msg) noexcept;

    void decode(const google::protobuf::Int64Value &msg,
                std::int64_t *n) noexcept;

    void encode(std::uint32_t n,
                google::protobuf::UInt32Value *msg) noexcept;

    void decode(const google::protobuf::UInt32Value &msg,
                std::uint32_t *n) noexcept;

    void encode(std::int32_t n,
                google::protobuf::Int32Value *msg) noexcept;

    void decode(const google::protobuf::Int32Value &msg,
                std::int32_t *n) noexcept;

    void encode(bool b,
                google::protobuf::BoolValue *msg) noexcept;

    void decode(const google::protobuf::BoolValue &msg,
                bool *b) noexcept;

    void encode(const core::types::Bytes &b,
                google::protobuf::BytesValue *msg) noexcept;

    void decode(const google::protobuf::BytesValue &msg,
                core::types::Bytes *b) noexcept;

    //==========================================================================
    // Timestamp encoding/decoding

    /// @brief
    ///     Encodecore::dt::TimePoint (a.k.a. std::chrono::system_clock::time_point)
    ///     to Google Protobuf Timestamp message.

    void encode(const core::dt::TimePoint &tp,
                google::protobuf::Timestamp *ts) noexcept;

    void decode(const google::protobuf::Timestamp &ts,
                core::dt::TimePoint *tp) noexcept;

    //==========================================================================
    // Duration encoding/decoding

    void encode(const core::dt::Duration &duration,
                google::protobuf::Duration *msg) noexcept;

    void decode(const google::protobuf::Duration &msg,
                core::dt::Duration *duration) noexcept;

    //==========================================================================
    // Google variant value encoding/decoding

    void encode(const core::types::Value &value,
                google::protobuf::Value *msg) noexcept;

    void decode(const google::protobuf::Value &msg,
                core::types::Value *value) noexcept;

    //==========================================================================
    // Google variant struct encoding/decoding

    void encode(const core::types::KeyValueMap &kvmap,
                google::protobuf::Struct *msg) noexcept;

    void decode(const google::protobuf::Struct &msg,
                core::types::KeyValueMap *kvmap) noexcept;

    void encode(const core::types::TaggedValueList &tvlist,
                google::protobuf::Struct *msg) noexcept;

    void decode(const google::protobuf::Struct &msg,
                core::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // Google variant list encoding/decoding

    void encode(const core::types::ValueList &list,
                google::protobuf::ListValue *msg) noexcept;

    void decode(const google::protobuf::ListValue &msg,
                core::types::ValueList *list) noexcept;

}  // namespace protobuf
