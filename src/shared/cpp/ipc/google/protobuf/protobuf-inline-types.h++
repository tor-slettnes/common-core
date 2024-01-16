/// -*- c++ -*-
//==============================================================================
/// @file protobuf-inline-types.h++
/// @brief Convenience templates for ProtoBuf encoding/decoding
/// @author Tor Slettnes <tor@slett.net>
///
/// The purpose of this file is to allow wrapper semantics like the following
/// around the respecitve methods protobuf::encode(nativeobject, protobufmessage)
///  and protobuf::decode(protobufmessage, nativeobject):
///   -  auto protomessage = cc::protobuf::encoded<ProtoBufType>(nativeobject);
///   -  auto nativeobject = cc::protobuf::decoded<NativeType>(protomessage);
/// Due to an apparent bug in the GNU C++ compiler, these function templates
/// must be declared AFTER the corresponding basic functions, as they do not
/// seem to be privy to overload resolution for methods declared in subsequently
/// included files. Consequently, you should only include this file directly in
/// your `.cc` source file, and only after any other `protobuf-*.h` imports.
//==============================================================================

#pragma once
#include <google/protobuf/message.h>
#include <vector>

namespace cc::protobuf
{
    //==========================================================================
    // Decode repeated fields

    template <class ProtoType, class NativeType>
    void encode_list(const typename NativeType::const_iterator &begin,
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

    template <class NativeType, class ProtoType>
    void decode_vector(const google::protobuf::RepeatedPtrField<ProtoType> &items,
                       std::vector<NativeType> *vector)
    {
        vector->clear();
        vector->reserve(items.size());
        for (const ProtoType &item : items)
        {
            vector->push_back(item);
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

    //==========================================================================

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline void from_bytes(
        const ByteArray &bytes,
        ProtoBufType *msg)
    {
        msg->ParseFromArray(bytes.data(), bytes.size());
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline ProtoBufType from_bytes(
        const ByteArray &bytes)
    {
        ProtoBufType msg;
        msg.ParseFromArray(bytes.data(), bytes.size());
        return msg;
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline void from_packed_string(
        const std::string &packed_string,
        ProtoBufType *msg)
    {
        msg->ParseFromString(packed_string);
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline ProtoBufType from_packed_string(
        const std::string &packed_string)
    {
        ProtoBufType msg;
        msg.ParseFromString(packed_string);
        return msg;
    }

}  // namespace cc::protobuf
