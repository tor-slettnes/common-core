/// -*- c++ -*-
//==============================================================================
/// @file protobuf-enum.h++
/// @brief Convenience templates for ProtoBuf enum encoding/decoding
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/symbolmap.h++"

#include <google/protobuf/message.h>

#include <string>
#include <vector>
#include <optional>

namespace cc::protobuf
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
        const std::string &name,
        const google::protobuf::EnumDescriptor *enum_desc,
        const std::string &delimiter = "_");

    std::string common_prefix(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter = "_");

    std::size_t common_prefix_length(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter = "_");


    template <class EnumType>
    core::types::SymbolMap<EnumType> symbol_map(
        const google::protobuf::EnumDescriptor *desc,
        const std::optional<std::string> &delimiter = "_")
    {
        std::size_t prefix_length = 0;
        if (delimiter)
        {
            prefix_length = common_prefix_length(desc, *delimiter);
        }

        core::types::SymbolMap<EnumType> symbol_map;
        for (int i = 0; i < desc->value_count(); i++)
        {
            std::string symbol = desc->value(i)->name();
            symbol_map.insert_or_assign(
                static_cast<EnumType>(i),
                symbol.substr(prefix_length));
        }
        return symbol_map;
    }


}  // namespace cc::protobuf
