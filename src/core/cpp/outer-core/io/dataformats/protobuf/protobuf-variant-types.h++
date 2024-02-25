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

    void encode(const shared::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                shared::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const shared::types::TaggedValue &tv,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                shared::types::TaggedValue *tv) noexcept;

    void encode(const shared::types::Tag &tag,
                const shared::types::Value &value,
                CC::Variant::Value *msg) noexcept;

    void decode(const CC::Variant::Value &msg,
                std::string *tag,
                shared::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    /// \brief
    ///    Encode shared::types::TaggedValueList to CC::Variant::ValueList protobuf message
    void encode(const shared::types::TaggedValueList &tvlist,
                CC::Variant::ValueList *msg) noexcept;

    /// \brief
    ///    Decode a CC::Variant::Value vector into shared::types::TaggeValueList.
    void decode(const CC::Variant::ValueList &msg,
                shared::types::TaggedValueList *tvlist) noexcept;

    /// \brief
    ///    Encode shared::types::TaggedValueList to a repeated CC::Variant::Valuef field.
    void encode(const shared::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept;

    /// \brief
    ///    Decode a repeated CC::Variant::Valuef field to TaggedValueList
    void decode(const RepeatedValue &msgs,
                shared::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const shared::types::KeyValueMap &map,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                shared::types::KeyValueMap *map) noexcept;

    void encode(const shared::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept;

    void decode(const RepeatedValue &msgs,
                shared::types::KeyValueMap *kvmap) noexcept;

    void encode(const shared::types::KeyValueMap &map,
                google::protobuf::Map<std::string, CC::Variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, CC::Variant::Value> &nmsg,
                shared::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // VariantValueList encoding/decoding

    void encode(const shared::types::ValueList &list,
                CC::Variant::ValueList *msg) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                shared::types::ValueList *list) noexcept;

    void decode(const RepeatedValue &msgs,
                shared::types::ValueList *list) noexcept;

    void decode(const CC::Variant::ValueList &msg,
                shared::types::Value *value) noexcept;

}  // namespace shared::protobuf
