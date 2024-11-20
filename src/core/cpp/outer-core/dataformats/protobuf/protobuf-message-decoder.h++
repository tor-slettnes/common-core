/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message-decoder.h++
/// @brief Encode/decode routines for ProtoBuf types provided by Google
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include <google/protobuf/message.h>

namespace protobuf
{
    //--------------------------------------------------------------------------
    // MessageDecoder helper class

    class MessageDecoder
    {
    public:
        MessageDecoder(const google::protobuf::Message &msg,
                       bool enums_as_strings = true);

        core::types::Value to_value() const;

    private:
        core::types::KeyValueMapPtr to_kvmap() const;

        core::types::Value field_to_value(
            const google::protobuf::FieldDescriptor &fd) const;

        core::types::Value single_field_to_value(
            const google::protobuf::FieldDescriptor &fd) const;

        core::types::Value indexed_field_to_value(
            const google::protobuf::FieldDescriptor &fd,
            int repeat_index) const;

        core::types::ValueListPtr repeated_field_to_valuelist(
            const google::protobuf::FieldDescriptor &fd) const;

        core::types::KeyValueMapPtr mapped_field_to_kvmap(
            const google::protobuf::FieldDescriptor &fd) const;

        core::types::Value message_to_value(
            const google::protobuf::Message &msg) const;

    private:
        const google::protobuf::Message &msg;
        const google::protobuf::Reflection &ref;
        const google::protobuf::Descriptor &descriptor;
        bool enums_as_strings;
    };

    //--------------------------------------------------------------------------
    // to_value() method

    core::types::Value to_value(const google::protobuf::Message &msg,
                                bool enums_as_strings = true);

}  // namespace protobuf

/// Add C++ output stream support for ProtoBuf messages
namespace google::protobuf
{
    std::ostream &operator<<(std::ostream &stream, const Message &msg);
}
