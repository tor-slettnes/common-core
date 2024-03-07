/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.h++
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

namespace protobuf
{
    using RepeatedValue = google::protobuf::RepeatedPtrField<CC::Variant::Value>;

    //==========================================================================
    // Variant Value encoding/decoding

    void encode(const core::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const core::types::TaggedValue &tv,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                core::types::TaggedValue *tv) noexcept;

    void encode(const core::types::Tag &tag,
                const core::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                std::string *tag,
                core::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    /// \brief
    ///    Encode core::types::TaggedValueList to CC::Variant::ValueList protobuf message
    void encode(const core::types::TaggedValueList &tvlist,
                CC::Variant::ValueList *msg) noexcept;

    /// \brief
    ///    Decode a CC::Variant::Value vector into core::types::TaggeValueList.
    void decode(const CC::Variant::ValueList &msg,
                core::types::TaggedValueList *tvlist) noexcept;

    /// \brief
    ///    Encode core::types::TaggedValueList to a repeated CC::Variant::Valuef field.
    void encode(const core::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept;

    /// \brief
    ///    Decode a repeated CC::Variant::Valuef field to TaggedValueList
    void decode(const RepeatedValue &msgs,
                core::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const core::types::KeyValueMap &map,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                core::types::KeyValueMap *map) noexcept;

    void encode(const core::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept;

    void decode(const RepeatedValue &msgs,
                core::types::KeyValueMap *kvmap) noexcept;

    void encode(const core::types::KeyValueMap &map,
                google::protobuf::Map<std::string, CC::Variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, CC::Variant::Value> &nmsg,
                core::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // VariantValueList encoding/decoding

    void encode(const core::types::ValueList &list,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                core::types::ValueList *list) noexcept;

    void decode(const RepeatedValue &msgs,
                core::types::ValueList *list) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                core::types::Value *value) noexcept;

}  // namespace core::protobuf
