/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/protobuf/variant/variant.pb.h" // generated from `variant.proto`
#include "chrono/date-time.h++"
#include "types/value.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
{
    using RepeatedValue = google::protobuf::RepeatedPtrField<cc::protobuf::variant::Value>;
    using RepeatedTaggedValue = google::protobuf::RepeatedPtrField<cc::protobuf::variant::TaggedValue>;

    //==========================================================================
    // Variant Value encoding/decoding

    void encode(const core::types::Value &value,
                cc::protobuf::variant::Value *msg) noexcept;

    void decode(const cc::protobuf::variant::Value &msg,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const core::types::TaggedValue &tv,
                cc::protobuf::variant::TaggedValue *msg) noexcept;

    void decode(const cc::protobuf::variant::TaggedValue &msg,
                core::types::TaggedValue *tv) noexcept;

    void encode(const core::types::Tag &tag,
                const core::types::Value &value,
                cc::protobuf::variant::TaggedValue *msg) noexcept;

    void decode(const cc::protobuf::variant::TaggedValue &msg,
                std::string *tag,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    void encode(const core::types::TaggedValueList &tvlist,
                cc::protobuf::variant::TaggedValueList *msg) noexcept;

    void decode(const cc::protobuf::variant::TaggedValueList &msg,
                core::types::TaggedValueList *tvlist) noexcept;

    void encode(const core::types::TaggedValueList &tvlist,
                RepeatedTaggedValue *msg) noexcept;

    void decode(const RepeatedTaggedValue &msgs,
                core::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const core::types::KeyValueMap &map,
                cc::protobuf::variant::KeyValueMap *msg) noexcept;

    void decode(const cc::protobuf::variant::KeyValueMap &msg,
                core::types::KeyValueMap *map) noexcept;

    void encode(const core::types::KeyValueMap &map,
                google::protobuf::Map<std::string, cc::protobuf::variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, cc::protobuf::variant::Value> &msg,
                core::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // ValueList encoding/decoding

    void encode(const core::types::ValueList &list,
                cc::protobuf::variant::ValueList *msg) noexcept;

    void decode(const cc::protobuf::variant::ValueList &msg,
                core::types::ValueList *list) noexcept;

    void encode(const core::types::ValueList &list,
                RepeatedValue *msgs) noexcept;

    void decode(const RepeatedValue &msgs,
                core::types::ValueList *list) noexcept;

} // namespace protobuf
