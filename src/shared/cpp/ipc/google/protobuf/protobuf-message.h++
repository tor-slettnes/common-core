/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message.h++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/bytearray.h++"

#include <google/protobuf/message.h>

#include <ostream>

namespace cc::protobuf
{
    //==========================================================================
    // Message deserialization

    /// Convert a ProtoBuf message to a printable string.
    std::string to_string(const google::protobuf::Message &msg,
                          bool single_line = true);

    /// Convert a protobuf message to a serialized byte array
    void to_bytes(const google::protobuf::Message &msg, types::ByteArray *bytes);

    /// Convert a protobuf message to a serialized byte array
    types::ByteArray to_bytes(const google::protobuf::Message &msg);

    //==========================================================================
    // Message serialization

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline void to_message(
        const ByteArray &bytes,
        ProtoBufType *msg)
    {
        msg->ParseFromArray(bytes.data(), bytes.size());
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline ProtoBufType to_message(
        const ByteArray &bytes)
    {
        ProtoBufType msg;
        msg.ParseFromArray(bytes.data(), bytes.size());
        return msg;
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline void to_message(
        const std::string &packed_string,
        ProtoBufType *msg)
    {
        msg->ParseFromString(packed_string);
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline ProtoBufType to_message(
        const std::string &packed_string)
    {
        ProtoBufType msg;
        msg.ParseFromString(packed_string);
        return msg;
    }
}  // namespace cc::protobuf

/// Add C++ output stream support for ProtoBuf messages (by reference and by pointer)
namespace google
{
    namespace protobuf
    {
        std::ostream &operator<<(std::ostream &stream,
                                 const google::protobuf::Message &msg);
    }  // namespace protobuf
}  // namespace google
