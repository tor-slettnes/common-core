/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message.c++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-message.h++"

#include <google/protobuf/text_format.h>

namespace cc::protobuf
{
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
                  types::ByteArray *bytes)
    {
        std::size_t size = msg.ByteSizeLong();
        bytes->resize(size);
        msg.SerializeWithCachedSizesToArray(bytes->data());
    }

    /// Convert a protobuf message to a serialized byte array
    types::ByteArray to_bytes(const google::protobuf::Message &msg)
    {
        ByteArray bytes;
        to_bytes(msg, &bytes);
        return bytes;
    }

}  // namespace cc::protobuf

/// Add C++ output stream support for ProtoBuf messages (by reference and by pointer)
namespace google
{
    namespace protobuf
    {
        std::ostream &operator<<(std::ostream &stream,
                                 const google::protobuf::Message &msg)
        {
            stream << cc::protobuf::to_string(msg);
            return stream;
        }
    }  // namespace protobuf
}  // namespace google
