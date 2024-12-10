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
    /// Helper class to convert arbitrary ProtoBuf messages to a
    /// `core::types::Value` variant, e.g. for logging or printing.

    class MessageDecoder
    {
    public:
        /// Constructor.
        /// @param[in] msg
        ///     ProtoBuf message to decode.
        /// @param[in] enums_as_strings
        ///     Whether to convert `enum` values to strings rather than unsigned
        ///     integers.
        MessageDecoder(const google::protobuf::Message &msg,
                       bool enums_as_strings = true);

        /// Convert the ProtoBuf message to a `core::types::Value` variant.
        ///
        /// Well-known message types from Google as well as our custom `Value`
        /// and `ValueList` message types are fully decoded. Other message types
        /// are converted to a `core::types::KeyValueMap` instance, where
        /// individual fields are decomposed recursively.
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
        const google::protobuf::Reflection *reflection;
        const google::protobuf::Descriptor *descriptor;
        bool enums_as_strings;
    };

    //--------------------------------------------------------------------------
    // to_value() method

    core::types::Value to_value(const google::protobuf::Message &msg,
                                bool enums_as_strings = true);

} // namespace protobuf

/// Add C++ output stream support for ProtoBuf messages
namespace google::protobuf
{
    std::ostream &operator<<(std::ostream &stream, const Message &msg);
}
