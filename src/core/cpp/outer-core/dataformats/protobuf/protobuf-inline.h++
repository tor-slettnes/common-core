/// -*- c++ -*-
//==============================================================================
/// @file protobuf-inline.h++
/// @brief Convenience templates for ProtoBuf encoding/decoding
/// @author Tor Slettnes <tor@slett.net>
///
/// The purpose of this file is to allow wrapper semantics like the following
/// around the respecitve methods protobuf::encode(nativeobject, protobufmessage)
///  and protobuf::decode(protobufmessage, nativeobject):
///   -  auto protomessage = protobuf::encoded<ProtoBufType>(nativeobject);
///   -  auto nativeobject = protobuf::decoded<NativeType>(protomessage);
/// Due to an apparent bug in the GNU C++ compiler, these function templates
/// must be declared AFTER the corresponding basic functions, as they do not
/// seem to be privy to overload resolution for methods declared in subsequently
/// included files. Consequently, you should only include this file directly in
/// your `.shared` source file, and only after any other `protobuf-*.h` imports.
//==============================================================================

#pragma once
#include <google/protobuf/message.h>
#include <vector>

namespace protobuf
{
    //==========================================================================
    // Decode repeated fields

    template <class ProtoType, class NativeType>
    void encode_sequence(const typename NativeType::const_iterator &begin,
                         const typename NativeType::const_iterator &end,
                         google::protobuf::RepeatedPtrField<ProtoType> *items)
    {
        items->Clear();
        items->Reserve(std::distance(begin, end));
        for (typename NativeType::const_iterator it = begin; it != end; it++)
        {
            encode(*it, items->Add());
        }
    }

    template <class ProtoType, class NativeType>
    void encode_vector(const std::vector<NativeType> &vector,
                       google::protobuf::RepeatedPtrField<ProtoType> *items)
    {
        items->Clear();
        items->Reserve(vector.size());
        for (const NativeType &item : vector)
        {
            encode(item, items->Add());
        }
    }

    template <class NativeType, class ProtoType>
    void decode_to_vector(const google::protobuf::RepeatedPtrField<ProtoType> &items,
                          std::vector<NativeType> *vector)
    {
        vector->reserve(items.size());
        for (const ProtoType &item : items)
        {
            decode(item, &vector->emplace_back());
        }
    }

    template <class NativeType, class ProtoType>
    std::vector<NativeType> decode_to_vector(
        const google::protobuf::RepeatedPtrField<ProtoType> &items)
    {
        std::vector<NativeType> vector;
        vector.reserve(items.size());
        for (const ProtoType &item : items)
        {
            decode(item, &vector.emplace_back());
        }
        return vector;
    }

    template <class Type>
    void assign_to_vector(const google::protobuf::RepeatedPtrField<Type> &items,
                          std::vector<Type> *vector)
    {
        vector->assign(items.begin(), items.end());
    }

    template <class Type>
    std::vector<Type> assign_to_vector(const google::protobuf::RepeatedPtrField<Type> &items)
    {
        std::vector<Type> vector;
        assign_to_vector(items, &vector);
        return vector;
    }

    template <class Type>
    void assign_repeated(const std::vector<Type> &vector,
                         google::protobuf::RepeatedPtrField<Type> *repeated_value)
    {
        repeated_value->Clear();
        repeated_value->Reserve(vector.size());
        for (const Type &value : vector)
        {
            *repeated_value->Add() = value;
        }
    }

    //==========================================================================
    // Template to return converted value

    template <class ProtoType, class... Args>
    inline ProtoType encoded(const Args &...args)
    {
        ProtoType msg;
        encode(args..., &msg);
        return msg;
    }

    template <class NativeType, class... Args>
    inline NativeType decoded(const Args &...args) noexcept
    {
        NativeType value;
        decode(args..., &value);
        return value;
    }

    //==========================================================================
    // Encode/decode wrapper for std::shared_ptr<> references

    template <class ProtoType, class NativeType>
    void encode_shared(const std::shared_ptr<NativeType> &ref, ProtoType *msg)
    {
        if (ref)
        {
            encode(*ref, msg);
        }
    }

    template <class ProtoType, class NativeType>
    ProtoType encode_shared(const std::shared_ptr<NativeType> &ref)
    {
        ProtoType msg;
        encode_shared<ProtoType, NativeType>(ref, &msg);
        return msg;
    }

    template <class NativeType, class ProtoType>
    void decode_shared(const ProtoType &msg, std::shared_ptr<NativeType> *ref)
    {
        if (!*ref)
        {
            *ref = std::make_shared<NativeType>();
        }
        decode(msg, ref->get());
    }

    template <class NativeType, class ProtoType>
    std::shared_ptr<NativeType> decode_shared(const ProtoType &msg)
    {
        auto ref = std::make_shared<NativeType>();
        decode(msg, ref.get());
        return ref;
    }

}  // namespace protobuf
