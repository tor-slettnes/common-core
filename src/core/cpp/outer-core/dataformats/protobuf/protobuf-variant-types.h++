/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant.pb.h" // generated from `variant.proto`
#include "chrono/date-time.h++"
#include "types/value.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace protobuf
{
    using RepeatedValue = google::protobuf::RepeatedPtrField<cc::variant::Value>;

    //==========================================================================
    // Complex value encoding/decoding

    void encode(const core::types::complex &value,
                cc::variant::Complex *msg) noexcept;

    void decode(const cc::variant::Complex &msg,
                core::types::complex *value) noexcept;

    //==========================================================================
    // Variant Value encoding/decoding

    void encode(const core::types::Value &value,
                cc::variant::Value *msg) noexcept;

    void decode(const cc::variant::Value &msg,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const core::types::TaggedValue &tv,
                cc::variant::Value *msg) noexcept;

    void decode(const cc::variant::Value &msg,
                core::types::TaggedValue *tv) noexcept;

    void encode(const core::types::Tag &tag,
                const core::types::Value &value,
                cc::variant::Value *msg) noexcept;

    void decode(const cc::variant::Value &msg,
                std::string *tag,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    /// @brief
    ///    Encodecore::types::TaggedValueList to cc::variant::ValueList protobuf message
    void encode(const core::types::TaggedValueList &tvlist,
                cc::variant::ValueList *msg) noexcept;

    /// @brief
    ///    Decode a cc::variant::Value vector intocore::types::TaggeValueList.
    void decode(const cc::variant::ValueList &msg,
                core::types::TaggedValueList *tvlist) noexcept;

    /// @brief
    ///    Encodecore::types::TaggedValueList to a repeated cc::variant::Valuef field.
    void encode(const core::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept;

    /// @brief
    ///    Decode a repeated cc::variant::Valuef field to TaggedValueList
    void decode(const RepeatedValue &msgs,
                core::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const core::types::KeyValueMap &map,
                cc::variant::ValueList *msg) noexcept;

    void decode(const cc::variant::ValueList &msg,
                core::types::KeyValueMap *map) noexcept;

    void encode(const core::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept;

    void decode(const RepeatedValue &msgs,
                core::types::KeyValueMap *kvmap) noexcept;

    void encode(const core::types::KeyValueMap &map,
                google::protobuf::Map<std::string, cc::variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, cc::variant::Value> &nmsg,
                core::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // VariantValueList encoding/decoding

    void encode(const core::types::ValueList &list,
                cc::variant::ValueList *msg) noexcept;

    void decode(const cc::variant::ValueList &msg,
                core::types::ValueList *list) noexcept;

    void decode(const RepeatedValue &msgs,
                core::types::ValueList *list) noexcept;

    void decode(const cc::variant::ValueList &msg,
                core::types::Value *value) noexcept;

} // namespace protobuf
