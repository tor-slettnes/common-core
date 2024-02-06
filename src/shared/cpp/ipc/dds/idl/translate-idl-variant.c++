/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-variant.c++
/// @brief Encode/decode routines for common IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-variant.h++"
#include "translate-idl-inline.h++"

namespace cc::idl
{
    //==========================================================================
    // Encode/decode Value

    void encode(const cc::types::Value &value,
                CC::Variant::Value *idl) noexcept
    {
        switch (value.type())
        {
        case cc::types::ValueType::NONE:
            idl->_d() = CC::Variant::ValueType::VT_NONE;
            break;

        case cc::types::ValueType::BOOL:
            idl->value_bool(value.as_bool());
            break;

        case cc::types::ValueType::UINT:
            idl->value_uint(value.as_uint());
            break;

        case cc::types::ValueType::SINT:
            idl->value_sint(value.as_sint());
            break;

        case cc::types::ValueType::REAL:
            idl->value_real(value.as_real());
            break;

        case cc::types::ValueType::COMPLEX:
            encode(value.as_complex(), &idl->value_complex());
            break;

        case cc::types::ValueType::CHAR:
            idl->value_char(value.as_char());
            break;

        case cc::types::ValueType::STRING:
            idl->value_string(value.as_string());
            break;

        case cc::types::ValueType::BYTEVECTOR:
            idl->value_bytearray(value.as_bytevector());
            break;

        case cc::types::ValueType::TIMEPOINT:
            encode(value.as_timepoint(), &idl->value_timestamp());
            break;

        case cc::types::ValueType::DURATION:
            encode(value.as_duration(), &idl->value_duration());
            break;

        case cc::types::ValueType::VALUELIST:
            // idl->value_sequence(
            //     encoded_shared<CC::Variant::ValueList>(value.as_valuelist()).list());
            break;

        case cc::types::ValueType::KVMAP:
            // idl->value_keyvaluemap(
            //     encoded_shared<CC::Variant::TaggedValueList>(value.as_kvmap()).list());
            break;

        case cc::types::ValueType::TVLIST:
            // idl->value_taggedsequence(
            //     encoded_shared<CC::Variant::TaggedValueList>(value.as_tvlist()).list());
            break;
        }
    }

    void decode(const CC::Variant::Value &idl,
                cc::types::Value *value) noexcept
    {
        switch (idl._d())
        {
        case CC::Variant::ValueType::VT_NONE:
            value->emplace<std::monostate>();
            break;

        case CC::Variant::ValueType::VT_BOOL:
            value->emplace<bool>(idl.value_bool());
            break;

        case CC::Variant::ValueType::VT_CHAR:
            value->emplace<char>(idl.value_char());
            break;

        case CC::Variant::ValueType::VT_UINT:
            value->emplace<cc::types::largest_uint>(idl.value_uint());
            break;

        case CC::Variant::ValueType::VT_SINT:
            value->emplace<cc::types::largest_sint>(idl.value_sint());
            break;

        case CC::Variant::ValueType::VT_REAL:
            value->emplace<double>(idl.value_real());
            break;

        case CC::Variant::ValueType::VT_COMPLEX:
            value->emplace<cc::types::complex>(idl.value_complex().real(), idl.value_complex().imag());
            break;

        case CC::Variant::ValueType::VT_STRING:
            value->emplace<std::string>(idl.value_string());
            break;

        case CC::Variant::ValueType::VT_BYTEARRAY:
        {
            const auto &sequence = idl.value_bytearray();
            value->emplace<ByteVector>(sequence.cbegin(), sequence.cend());
            break;
        }

        case CC::Variant::ValueType::VT_TIMEPOINT:
            value->emplace<cc::dt::TimePoint>(decoded<cc::dt::TimePoint>(idl.value_timestamp()));
            break;

        case CC::Variant::ValueType::VT_DURATION:
            value->emplace<cc::dt::Duration>(decoded<cc::dt::Duration>(idl.value_duration()));
            break;

            // case CC::Variant::ValueType::VT_SEQUENCE:
            // {
            //     const auto &sequence = idl.value_sequence();
            //     *value = decoded_shared<cc::types::ValueList>(
            //         sequence.cbegin(),
            //         sequence.cend());
            //     break;
            // }

            // case CC::Variant::ValueType::VT_KEYVALUEMAP:
            // {
            //     const auto &sequence = idl.value_keyvaluemap();
            //     *value = decoded_shared<cc::types::KeyValueMap>(
            //                  sequence.cbegin(),
            //                  sequence.cend());
            //     break;
            // }

            // case CC::Variant::ValueType::VT_TAGGEDSEQUENCE:
            // {
            //     const auto &sequence = idl.value_taggedsequence();
            //     *value = decoded_shared<cc::types::TaggedValueList>(
            //         sequence.cbegin(),
            //         sequence.cend());
            //     break;
            // }
        }
    }

    //==========================================================================
    // Encode/decode TaggedValue

    void encode(const cc::types::TaggedValue &native,
                CC::Variant::TaggedValue *idl) noexcept
    {
        encode(native.first, native.second, idl);
    }

    void decode(const CC::Variant::TaggedValue &idl,
                cc::types::TaggedValue *native) noexcept
    {
        *native = {idl.tag(), decoded<cc::types::Value>(idl.value())};
    }

    void encode(const cc::types::Tag &tag,
                const cc::types::Value &value,
                CC::Variant::TaggedValue *idl) noexcept
    {
        idl->tag(tag.value_or(""));
        idl->value(encoded<CC::Variant::Value>(value));
    }

    void decode(const CC::Variant::TaggedValue &idl,
                std::string *tag,
                cc::types::Value *value) noexcept
    {
        *tag = idl.tag();
        decode(idl.value(), value);
    }

    //==========================================================================
    // Encode/decode ValueList

    void encode(const cc::types::ValueList &native,
                CC::Variant::ValueList *idl) noexcept
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const cc::types::Value &value : native)
        {
            encode(value, &*it++);
        }
    }

    void decode(const CC::Variant::ValueList &idl,
                cc::types::ValueList *native) noexcept
    {
        native->clear();
        native->reserve(idl.list().size());
        for (const auto &idl_value : idl.list())
        {
            decode(idl_value, &native->emplace_back());
        }
    }

    void decode(std::vector<CC::Variant::Value>::const_iterator begin,
                std::vector<CC::Variant::Value>::const_iterator end,
                cc::types::ValueList *native) noexcept
    {
        native->clear();
        native->reserve(std::distance(begin, end));
        for (auto it = begin; it != end; it++)
        {
            decode(*it, &native->emplace_back());
        }
    }

    //==========================================================================
    // Encode/decode TaggedValueList

    void encode(const cc::types::TaggedValueList &native,
                CC::Variant::TaggedValueList *idl) noexcept
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const cc::types::TaggedValue &tv : native)
        {
            encode(tv, &*it++);
        }
    }

    void decode(const CC::Variant::TaggedValueList &idl,
                cc::types::TaggedValueList *native) noexcept
    {
        native->clear();
        native->reserve(idl.list().size());
        for (const CC::Variant::TaggedValue &tv : idl.list())
        {
            decode(tv, &native->emplace_back());
        }
    }

    void decode(std::vector<CC::Variant::TaggedValue>::const_iterator begin,
                std::vector<CC::Variant::TaggedValue>::const_iterator end,
                cc::types::TaggedValueList *native) noexcept
    {
        native->clear();
        native->reserve(std::distance(begin, end));
        for (auto it = begin; it != end; it++)
        {
            decode(*it, &native->emplace_back());
        }
    }

    //==========================================================================
    // Encode/decode KeyValueMap

    void encode(const cc::types::KeyValueMap &native,
                CC::Variant::TaggedValueList *idl) noexcept
    {
        idl->list().resize(native.size());
        auto it = idl->list().begin();
        for (const auto &[key, value] : native)
        {
            encode(key, value, &*it++);
        }
    }

    void decode(const CC::Variant::TaggedValueList &idl,
                cc::types::KeyValueMap *native) noexcept
    {
        native->clear();
        for (const auto &tv : idl.list())
        {
            decode(tv.value(), &(*native)[tv.tag()]);
        }
    }

}  // namespace cc::idl
