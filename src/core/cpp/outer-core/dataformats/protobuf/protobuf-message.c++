/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-message.h++"
#include "string/misc.h++"

#include <google/protobuf/text_format.h>
#include <google/protobuf/util/message_differencer.h>

#include <iostream>

using google::protobuf::util::MessageDifferencer;

namespace protobuf
{
    const google::protobuf::Empty empty;

    //==========================================================================
    // Message (de)serialization

    /// Convert a ProtoBuf message to a printable string.
    std::string to_string(const google::protobuf::Message &msg,
                          bool single_line)
    {
        std::string repr;
        google::protobuf::TextFormat::Printer printer;
        printer.SetSingleLineMode(single_line);
        printer.PrintToString(msg, &repr);
        return repr;
    }

    /// Convert a protobuf message to a serialized byte array
    void to_bytes(const google::protobuf::Message &msg,
                  core::types::ByteVector *bytes)
    {
        std::size_t size = msg.ByteSizeLong();
        bytes->resize(size);
        msg.SerializeWithCachedSizesToArray(bytes->data());
    }

    /// Convert a protobuf message to a serialized byte array
    core::types::ByteVector to_bytes(const google::protobuf::Message &msg)
    {
        ByteVector bytes;
        to_bytes(msg, &bytes);
        return bytes;
    }

}  // namespace protobuf

/// Add C++ output stream support for ProtoBuf messages (by reference and by pointer)
namespace google::protobuf
{
    bool operator==(const Message &left, const Message &right)
    {
        return MessageDifferencer::Equivalent(left, right);
    }

    bool operator!=(const Message &left, const Message &right)
    {
        return !MessageDifferencer::Equivalent(left, right);
    }

    std::ostream &operator<<(std::ostream &stream, const Message &msg)
    {
        stream << ::protobuf::to_string(msg);
        return stream;
    }
}  // namespace google::protobuf
