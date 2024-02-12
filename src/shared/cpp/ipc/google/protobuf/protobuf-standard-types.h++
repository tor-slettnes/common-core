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
    using google::protobuf::Empty;

    //==========================================================================
    // Simple protobuf types

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

    void encode(const shared::types::Bytes &b,
                google::protobuf::BytesValue *msg) noexcept;

    void decode(const google::protobuf::BytesValue &msg,
                shared::types::Bytes *b) noexcept;

    //==========================================================================
    // Timestamp encoding/decoding

    /// \brief
    ///     Encode shared::dt::TimePoint (a.k.a. std::chrono::system_clock::time_point)
    ///     to Google Protobuf Timestamp message.

    void encode(const shared::dt::TimePoint &tp,
                google::protobuf::Timestamp *ts) noexcept;

    void decode(const google::protobuf::Timestamp &ts,
                shared::dt::TimePoint *tp) noexcept;

    //==========================================================================
    // Duration encoding/decoding

    void encode(const shared::dt::Duration &duration,
                google::protobuf::Duration *msg) noexcept;

    void decode(const google::protobuf::Duration &msg,
                shared::dt::Duration *duration) noexcept;

    //==========================================================================
    // Google variant value encoding/decoding

    void encode(const shared::types::Value &value,
                google::protobuf::Value *msg) noexcept;

    void decode(const google::protobuf::Value &msg,
                shared::types::Value *value) noexcept;

    //==========================================================================
    // Google variant struct encoding/decoding

    void encode(const shared::types::KeyValueMap &kvmap,
                google::protobuf::Struct *msg) noexcept;

    void decode(const google::protobuf::Struct &msg,
                shared::types::KeyValueMap *kvmap) noexcept;

    void encode(const shared::types::TaggedValueList &tvlist,
                google::protobuf::Struct *msg) noexcept;

    void decode(const google::protobuf::Struct &msg,
                shared::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // Google variant list encoding/decoding

    void encode(const shared::types::ValueList &list,
                google::protobuf::ListValue *msg) noexcept;

    void decode(const google::protobuf::ListValue &msg,
                shared::types::ValueList *list) noexcept;
}  // namespace shared::protobuf

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
