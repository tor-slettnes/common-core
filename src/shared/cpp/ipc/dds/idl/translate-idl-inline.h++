/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-inline.h++
/// @brief Convenience templates for IDL encoding/decoding
/// @author Tor Slettnes <tor@slett.net>
///
/// The purpose of this file is to allow wrapper semantics like the following
/// around the various overloaded encode()/decode() methods:
///     ```
///     auto idl_object = encoded<IdlType>(native_object);
///     auto native_object = decoded<NativeType>(idl_object);
///     ```
/// Due to an apparent bug in the GNU C++ compiler, these function templates
/// must be declared AFTER the corresponding basic functions, as they do not
/// seem to be privy to overload resolution for methods declared in subsequently
/// included files. Consequently, you should only include this file directly in
/// your `.cpp` source file, and only after any other `translate-*.hpp` imports.
//==============================================================================

#pragma once
#include <dds/core/types.hpp>

namespace idl
{
    //==========================================================================
    // Template to support encode()/decode() semantics for identical types

    template <class SimpleType>
    inline void encode(const SimpleType &native, SimpleType *idl)
    {
        *idl = native;
    }

    template <class SimpleType>
    inline void decode(const SimpleType &idl, SimpleType *native)
    {
        *native = idl;
    }

    //==========================================================================
    // Template to return converted value

    template <class IdlType, class... Args>
    inline IdlType encoded(Args &&...args)
    {
        IdlType obj;
        encode(args..., &obj);
        return obj;
    }

    template <class NativeType, class... Args>
    inline NativeType decoded(Args &&...args) noexcept
    {
        NativeType value;
        decode(args..., &value);
        return value;
    }

    //==========================================================================
    // Encode/decode wrapper for std::shared_ptr<> references

    template <class IdlType, class NativeType>
    void encode_shared(std::shared_ptr<NativeType> ref, IdlType *obj)
    {
        if (ref)
        {
            encode(*ref, obj);
        }
    }

    template <class IdlType, class NativeType>
    IdlType encoded_shared(const std::shared_ptr<NativeType> &ref)
    {
        IdlType obj;
        if (ref)
        {
            encode(*ref, &obj);
        }
        return obj;
    }

    template <class NativeType, class IdlType>
    void decode_shared(const IdlType &obj, std::shared_ptr<NativeType> *ref)
    {
        if (!*ref)
        {
            *ref = std::make_shared<NativeType>();
        }
        decode(obj, ref->get());
    }

    template <class NativeType, class... Args>
    std::shared_ptr<NativeType> decoded_shared(Args &&...args)
    {
        auto ref = std::make_shared<NativeType>();
        decode(args..., ref.get());
        return ref;
    }

    //==========================================================================
    // Encode/decode Built-in types

    template <class T>
    rti::core::optional<T> encode_optional(const std::optional<T> &native)
    {
        return native.value_or({});
    }

    template <class T>
    std::optional<T> decode_optional(const rti::core::optional<T> &rti)
    {
        if (rti.has_value())
        {
            return rti.value();
        }
        else
        {
            return {};
        }
    }

}  // namespace idl
