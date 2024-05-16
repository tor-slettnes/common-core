/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-variant.h++
/// @brief Encode/decode routines for common IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-common.h++"
#include "variant-types.hpp"  // generated from `variant-types.idl`
#include "types/value.h++"

namespace idl
{
    //==========================================================================
    // Encode/decode VariantValue

    void encode(const cc::types::Value &native,
                CC::Variant::Value *idl) noexcept;

    void decode(const CC::Variant::Value &idl,
                cc::types::Value *native) noexcept;

    //==========================================================================
    // Encode/decode TaggedValue

    void encode(const cc::types::TaggedValue &native,
                CC::Variant::TaggedValue *idl) noexcept;

    void decode(const CC::Variant::TaggedValue &idl,
                cc::types::TaggedValue *native) noexcept;

    void encode(const cc::types::Tag &tag,
                const cc::types::Value &value,
                CC::Variant::TaggedValue *idl) noexcept;

    void decode(const CC::Variant::TaggedValue &idl,
                std::string *tag,
                cc::types::Value *value) noexcept;

    //==========================================================================
    // Encode/decode ValueList

    void encode(const cc::types::ValueList &native,
                CC::Variant::ValueList *idl) noexcept;

    void decode(const CC::Variant::ValueList &idl,
                cc::types::ValueList *native) noexcept;

    void decode(std::vector<CC::Variant::Value>::const_iterator begin,
                std::vector<CC::Variant::Value>::const_iterator end,
                cc::types::ValueList *native) noexcept;

    //==========================================================================
    // Encode/decode TaggedValueList

    void encode(const cc::types::TaggedValueList &native,
                CC::Variant::TaggedValueList *idl) noexcept;

    void decode(const CC::Variant::TaggedValueList &idl,
                cc::types::TaggedValueList *native) noexcept;

    void decode(std::vector<CC::Variant::TaggedValue>::const_iterator begin,
                std::vector<CC::Variant::TaggedValue>::const_iterator end,
                cc::types::TaggedValueList *native) noexcept;

    //==========================================================================
    // Encode/decode KeyValueMap

    void encode(const cc::types::KeyValueMap &native,
                CC::Variant::TaggedValueList *idl) noexcept;

    void decode(const CC::Variant::TaggedValueList &idl,
                cc::types::KeyValueMap *native) noexcept;

}  // namespace idl
