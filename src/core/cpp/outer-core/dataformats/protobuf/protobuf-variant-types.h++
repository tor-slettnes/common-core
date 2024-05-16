/// -*- c++ -*-
//==============================================================================
/// @file protobuf-variant-types.h++
/// @brief Encode/decode routines for common ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant.pb.h"  // generated from `variant.proto`
#include "chrono/date-time.h++"
#include "types/value.h++"

/// ProtoBuf message conversions.
///
/// These are convenience methods to convert to ("encode...()") or from
/// ("decode...()") "common" ProtoBuf messages (including those defined in
/// "common_types.proto").

namespace cc::io::proto
{
    using RepeatedValue = google::protobuf::RepeatedPtrField<cc::protobuf::variant::Value>;

    //==========================================================================
    // Variant Value encoding/decoding

    void encode(const cc::types::Value &value,
                cc::protobuf::variant::Value *msg) noexcept;

    void decode(const cc::protobuf::variant::Value &msg,
               cc::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValue encoding/decoding

    void encode(const cc::types::TaggedValue &tv,
                cc::protobuf::variant::Value *msg) noexcept;

    void decode(const cc::protobuf::variant::Value &msg,
               cc::types::TaggedValue *tv) noexcept;

    void encode(const cc::types::Tag &tag,
                const cc::types::Value &value,
                cc::protobuf::variant::Value *msg) noexcept;

    void decode(const cc::protobuf::variant::Value &msg,
                std::string *tag,
               cc::types::Value *value) noexcept;

    //==========================================================================
    // TaggedValueList encoding/decoding

    /// \brief
    ///    Encodecc::types::TaggedValueList to cc::protobuf::variant::ValueList protobuf message
    void encode(const cc::types::TaggedValueList &tvlist,
                cc::protobuf::variant::ValueList *msg) noexcept;

    /// \brief
    ///    Decode a cc::protobuf::variant::Value vector intocc::types::TaggeValueList.
    void decode(const cc::protobuf::variant::ValueList &msg,
               cc::types::TaggedValueList *tvlist) noexcept;

    /// \brief
    ///    Encodecc::types::TaggedValueList to a repeated cc::protobuf::variant::Valuef field.
    void encode(const cc::types::TaggedValueList &tvlist,
                RepeatedValue *msg) noexcept;

    /// \brief
    ///    Decode a repeated cc::protobuf::variant::Valuef field to TaggedValueList
    void decode(const RepeatedValue &msgs,
               cc::types::TaggedValueList *tvlist) noexcept;

    //==========================================================================
    // KeyValueMap encoding/decoding

    void encode(const cc::types::KeyValueMap &map,
                cc::protobuf::variant::ValueList *msg) noexcept;

    void decode(const cc::protobuf::variant::ValueList &msg,
               cc::types::KeyValueMap *map) noexcept;

    void encode(const cc::types::KeyValueMap &map,
                RepeatedValue *msg) noexcept;

    void decode(const RepeatedValue &msgs,
               cc::types::KeyValueMap *kvmap) noexcept;

    void encode(const cc::types::KeyValueMap &map,
                google::protobuf::Map<std::string, cc::protobuf::variant::Value> *msg) noexcept;

    void decode(const google::protobuf::Map<std::string, cc::protobuf::variant::Value> &nmsg,
               cc::types::KeyValueMap *map) noexcept;

    //==========================================================================
    // VariantValueList encoding/decoding

    void encode(const cc::types::ValueList &list,
                cc::protobuf::variant::ValueList *msg) noexcept;

    void decode(const cc::protobuf::variant::ValueList &msg,
               cc::types::ValueList *list) noexcept;

    void decode(const RepeatedValue &msgs,
               cc::types::ValueList *list) noexcept;

    void decode(const cc::protobuf::variant::ValueList &msg,
               cc::types::Value *value) noexcept;

}  // cc::io::proto
