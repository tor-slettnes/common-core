/// -*- c++ -*-
//==============================================================================
/// @file protobuf-enum.h++
/// @brief Convenience templates for ProtoBuf enum encoding/decoding
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <google/protobuf/message.h>
#include <vector>
#include <optional>

namespace protobuf
{
    std::vector<std::string> enum_names(
        const google::protobuf::EnumDescriptor *desc,
        const std::optional<std::string> &delimiter = "_");

    std::string enum_name(
        int enum_value,
        const google::protobuf::EnumDescriptor *enum_desc,
        const std::optional<std::string> &delimiter = "_",
        const std::string &fallback = "");

    std::optional<int> enum_value(
        std::string name,
        const google::protobuf::EnumDescriptor *enum_desc,
        const std::optional<std::string> &delimiter = "_");

    std::string common_prefix(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter = "_");

    std::size_t common_prefix_length(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter = "_");

}  // namespace protobuf
