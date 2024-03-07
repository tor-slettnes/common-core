/// -*- c++ -*-
//==============================================================================
/// @file protobuf-standard-types.h++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
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

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
{
    using google::protobuf::Message;
    using google::protobuf::Empty;
    using google::protobuf::BoolValue;
    using google::protobuf::FloatValue;
    using google::protobuf::DoubleValue;
    using google::protobuf::UInt64Value;
    using google::protobuf::Int64Value;
    using google::protobuf::UInt32Value;
    using google::protobuf::Int32Value;
    using google::protobuf::StringValue;
    using google::protobuf::BytesValue;
    using google::protobuf::Timestamp;
    using google::protobuf::Duration;
    using google::protobuf::Value;
    using google::protobuf::ListValue;
    using google::protobuf::Struct;
    using google::protobuf::Map;

    using IntValue = google::protobuf::Int64Value;
    using UIntValue = google::protobuf::UInt64Value;
    using RealValue = google::protobuf::DoubleValue;
    using RepeatedString = google::protobuf::RepeatedPtrField<std::string>;

    //==========================================================================
    // Simple protobuf types

    void encode(const std::string &s,
                protobuf::StringValue *msg) noexcept;
    void decode(const protobuf::StringValue &msg,
                std::string *s) noexcept;

    void encode(double r,
                protobuf::DoubleValue *msg) noexcept;

    void decode(const protobuf::DoubleValue &msg,
                double *r) noexcept;

    void encode(float r,
                protobuf::FloatValue *msg) noexcept;

    void decode(const protobuf::FloatValue &msg,
                float *r) noexcept;

    void encode(std::uint64_t n,
                protobuf::UInt64Value *msg) noexcept;

    void decode(const protobuf::UInt64Value &msg,
                std::uint64_t *n) noexcept;

    void encode(std::int64_t n,
                protobuf::Int64Value *msg) noexcept;

    void decode(const protobuf::Int64Value &msg,
                std::int64_t *n) noexcept;

    void encode(std::uint32_t n,
                protobuf::UInt32Value *msg) noexcept;

    void decode(const protobuf::UInt32Value &msg,
                std::uint32_t *n) noexcept;

    void encode(std::int32_t n,
                protobuf::Int32Value *msg) noexcept;

    void decode(const protobuf::Int32Value &msg,
                std::int32_t *n) noexcept;

    void encode(bool b,
                protobuf::BoolValue *msg) noexcept;

    void decode(const protobuf::BoolValue &msg,
                bool *b) noexcept;

    void encode(const core::types::Bytes &b,
                protobuf::BytesValue *msg) noexcept;

    void decode(const protobuf::BytesValue &msg,
                core::types::Bytes *b) noexcept;

    //==========================================================================
    // Timestamp encoding/decoding

    /// \brief
    ///     Encode core::dt::TimePoint (a.k.a. std::chrono::system_clock::time_point)
    ///     to Google Protobuf Timestamp message.

    void encode(const core::dt::TimePoint &tp,
                protobuf::Timestamp *ts) noexcept;

    void decode(const protobuf::Timestamp &ts,
                core::dt::TimePoint *tp) noexcept;

    //==========================================================================
    // Duration encoding/decoding

    void encode(const core::dt::Duration &duration,
                protobuf::Duration *msg) noexcept;

    void decode(const protobuf::Duration &msg,
                core::dt::Duration *duration) noexcept;

    //==========================================================================
    // Google variant value encoding/decoding

    void encode(const core::types::Value &value,
                protobuf::Value *msg) noexcept;

    void decode(const protobuf::Value &msg,
                core::types::Value *value) noexcept;

    //==========================================================================
    // Google variant struct encoding/decoding

    void encode(const core::types::KeyValueMap &kvmap,
                protobuf::Struct *msg) noexcept;

    void decode(const protobuf::Struct &msg,
                core::types::KeyValueMap *kvmap) noexcept;

    void encode(const core::types::TaggedValueList &tvlist,
                protobuf::Struct *msg) noexcept;

    void decode(const protobuf::Struct &msg,
                core::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // Google variant list encoding/decoding

    void encode(const core::types::ValueList &list,
                protobuf::ListValue *msg) noexcept;

    void decode(const protobuf::ListValue &msg,
                core::types::ValueList *list) noexcept;
}  // namespace core::protobuf

// /// Add C++ output stream support for ProtoBuf messages (by reference and by pointer)
// namespace google
// {
//     namespace protobuf
//     {
//         inline std::ostream &operator<<(std::ostream &stream, const Message &msg)
//         {
//             stream << ::protobuf::to_string(msg);
//             return stream;
//         }
//     }  // namespace protobuf
// }  // namespace google
