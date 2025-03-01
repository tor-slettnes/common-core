/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message.h++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "protobuf-message-decoder.h++"
#include "types/bytevector.h++"
#include "string/stream.h++"

#include <google/protobuf/message.h>
#include <google/protobuf/empty.pb.h>

#include <ostream>

namespace protobuf
{
    extern const google::protobuf::Empty empty;

    //==========================================================================
    // Message deserialization

    /// Convert a ProtoBuf message to a printable string.
    std::string to_string(const google::protobuf::Message &msg,
                          bool single_line = true);

    /// Convert a protobuf message to a serialized byte array
    void to_bytes(const google::protobuf::Message &msg,
                  core::types::ByteVector *bytes);

    /// Convert a protobuf message to a serialized byte array
    core::types::ByteVector to_bytes(const google::protobuf::Message &msg);

    //==========================================================================
    // Message serialization

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline void to_message(
        const ByteVector &bytes,
        ProtoBufType *msg)
    {
        msg->ParseFromArray(bytes.data(), bytes.size());
    }

    /// Convert a serialized byte array to a ProtoBuf message of type ProtoBufType.
    template <class ProtoBufType>
    inline ProtoBufType to_message(
        const ByteVector &bytes)
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

}  // namespace protobuf


/// Additional convenience operators for ProtoBuf messages
namespace google::protobuf
{
    bool operator==(const Message &left, const Message &right);
    bool operator!=(const Message &left, const Message &right);

    // std::ostream &operator<<(std::ostream &stream, const Message &msg);

    template <class T>
    std::ostream &operator<<(std::ostream &stream,
                             const RepeatedPtrField<T> &ptr_field)
    {
        return core::stream::write_sequence(stream, ptr_field);
    }
}  // namespace google::protobuf
