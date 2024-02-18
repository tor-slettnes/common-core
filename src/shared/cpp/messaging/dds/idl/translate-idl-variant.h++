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

    void encode(const shared::types::Value &native,
                CC::Variant::Value *idl) noexcept;

    void decode(const CC::Variant::Value &idl,
                shared::types::Value *native) noexcept;

    //==========================================================================
    // Encode/decode TaggedValue

    void encode(const shared::types::TaggedValue &native,
                CC::Variant::TaggedValue *idl) noexcept;

    void decode(const CC::Variant::TaggedValue &idl,
                shared::types::TaggedValue *native) noexcept;

    void encode(const shared::types::Tag &tag,
                const shared::types::Value &value,
                CC::Variant::TaggedValue *idl) noexcept;

    void decode(const CC::Variant::TaggedValue &idl,
                std::string *tag,
                shared::types::Value *value) noexcept;

    //==========================================================================
    // Encode/decode ValueList

    void encode(const shared::types::ValueList &native,
                CC::Variant::ValueList *idl) noexcept;

    void decode(const CC::Variant::ValueList &idl,
                shared::types::ValueList *native) noexcept;

    void decode(std::vector<CC::Variant::Value>::const_iterator begin,
                std::vector<CC::Variant::Value>::const_iterator end,
                shared::types::ValueList *native) noexcept;

    //==========================================================================
    // Encode/decode TaggedValueList

    void encode(const shared::types::TaggedValueList &native,
                CC::Variant::TaggedValueList *idl) noexcept;

    void decode(const CC::Variant::TaggedValueList &idl,
                shared::types::TaggedValueList *native) noexcept;

    void decode(std::vector<CC::Variant::TaggedValue>::const_iterator begin,
                std::vector<CC::Variant::TaggedValue>::const_iterator end,
                shared::types::TaggedValueList *native) noexcept;

    //==========================================================================
    // Encode/decode KeyValueMap

    void encode(const shared::types::KeyValueMap &native,
                CC::Variant::TaggedValueList *idl) noexcept;

    void decode(const CC::Variant::TaggedValueList &idl,
                shared::types::KeyValueMap *native) noexcept;

}  // namespace idl