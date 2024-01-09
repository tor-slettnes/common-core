/// -*- c++ -*-
//==============================================================================
/// @file translate-protobuf-variant.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant_types.pb.h"  // generated from `common-types.proto`
#include "chrono/date-time.h++"
#include "types/value.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace cc::protobuf
{
    using RepeatedValue = google::protobuf::RepeatedPtrField<CC::Variant::Value>;

    //==========================================================================
    // Variant Value encoding/decoding

    void encode(const cc::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                cc::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const cc::types::TaggedValue &tv,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                cc::types::TaggedValue *tv) noexcept;

    void encode(const cc::types::Tag &tag,
                const cc::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                std::string *tag,
                cc::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    /// \brief
    ///    Encode cc::types::TaggedValueList to CC::Variant::ValueList protobuf message
    void encode(const cc::types::TaggedValueList &tvlist,
                CC::Variant::ValueList *msg) noexcept;

    /// \brief
    ///    Decode a CC::Variant::Value vector into cc::types::TaggeValueList.
    void decode(const CC::Variant::ValueList &msg,
                cc::types::TaggedValueList *tvlist) noexcept;

    /// \brief
    ///    Encode cc::types::TaggedValueList to a repeated CC::Variant::Valuef field.
    void encode(const cc::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept;

    /// \brief
    ///    Decode a repeated CC::Variant::Valuef field to TaggedValueList
    void decode(const RepeatedValue &msgs,
                cc::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const cc::types::KeyValueMap &map,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                cc::types::KeyValueMap *map) noexcept;

    void encode(const cc::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept;

    void decode(const RepeatedValue &msgs,
                cc::types::KeyValueMap *kvmap) noexcept;

    void encode(const cc::types::KeyValueMap &map,
                google::protobuf::Map<std::string, CC::Variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, CC::Variant::Value> &nmsg,
                cc::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // VariantValueList encoding/decoding

    void encode(const cc::types::ValueList &list,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                cc::types::ValueList *list) noexcept;

    void decode(const RepeatedValue &msgs,
                cc::types::ValueList *list) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                cc::types::Value *value) noexcept;

}  // namespace cc::protobuf
