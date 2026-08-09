/// -*- c++ -*-
//==============================================================================
/// @file protobuf-message-encoder.h++
/// @brief Populate ProtoBuf message from variant Value instances
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/value.h++"
#include "protobuf-message.h++"

#include <google/protobuf/message.h>

namespace cc::protobuf
{
    class MessageEncoder
    {
        using This = MessageEncoder;

    public:
        static bool encode_message(
            const core::types::Value& value,
            ::google::protobuf::Message* msg);

    private:
        static bool encode_wellknown(
            const core::types::Value& value,
            ::google::protobuf::Message* msg);

        static bool encode_custom(
            const core::types::Value& value,
            ::google::protobuf::Message* msg);

        static bool encode_field(
            const core::types::Value& value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool encode_single_field(
            const core::types::Value& value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool encode_repeated_field(
            const core::types::Value& value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool encode_repeated_element(
            const core::types::Value& value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static bool encode_mapped_field(
            const core::types::Value& value,
            google::protobuf::Message* msg,
            const google::protobuf::FieldDescriptor* fd);

        static std::optional<int> encode_enum_value(
            const core::types::Value& value,
            const google::protobuf::EnumDescriptor* ed);
    };

    bool encode_to_message(
        const core::types::Value& value,
        ::google::protobuf::Message* msg);

    template <class MessageType>
    MessageType to_message(
        const core::types::Value& value)
    {
        MessageType msg;
        MessageEncoder::encode_message(value, &msg);
        return msg;
    }

}  // namespace cc::protobuf
