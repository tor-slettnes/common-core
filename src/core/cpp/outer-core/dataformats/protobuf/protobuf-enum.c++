/// -*- c++ -*-
//==============================================================================
/// @file protobuf-enum.c++
/// @brief Convenience templates for ProtoBuf enum encoding/decoding
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-enum.h++"
#include "string/misc.h++"

namespace protobuf
{
    std::vector<std::string> enum_names(
        const google::protobuf::EnumDescriptor *desc,
        const std::optional<std::string> &delimiter)
    {
        std::vector<std::string> symbols;
        symbols.reserve(desc->value_count());
        for (int i = 0; i < desc->value_count(); i++)
        {
            symbols.push_back(desc->value(i)->name());
        }

        if (delimiter)
        {
            std::size_t prefix_length = common_prefix_length(desc, *delimiter);
            for (std::string &symbol : symbols)
            {
                symbol = symbol.substr(prefix_length);
            }
        }

        return symbols;
    }

    std::string enum_name(
        int enum_value,
        const google::protobuf::EnumDescriptor *enum_desc,
        const std::optional<std::string> &delimiter,
        const std::string &fallback)
    {
        if (const auto *value = enum_desc->FindValueByNumber(enum_value))
        {
            std::string name = value->name();
            if (delimiter)
            {
                name = name.substr(common_prefix_length(enum_desc, *delimiter));
            }
            return name;
        }
        else
        {
            return fallback;
        }
    }

    std::optional<int> enum_value(
        std::string name,
        const google::protobuf::EnumDescriptor *enum_desc,
        const std::optional<std::string> &delimiter)
    {
        std::string prefix =
            delimiter ? common_prefix(enum_desc, *delimiter)
                      : "";

        if (const auto *vd = enum_desc->FindValueByName(prefix + name))
        {
            return vd->number();
        }
        else
        {
            return {};
        }
    }

    std::string common_prefix(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter)
    {
        static std::unordered_map<
            std::string,
            std::unordered_map<const google::protobuf::EnumDescriptor *, std::string>>
            cached_prefixes;

        try
        {
            return cached_prefixes[delimiter].at(desc);
        }
        catch (const std::out_of_range &)
        {
            std::vector<std::string> symbols;
            symbols.reserve(desc->value_count());
            for (int i = 0; i < desc->value_count(); i++)
            {
                symbols.push_back(desc->value(i)->name());
            }

            auto [it, inserted] = cached_prefixes[delimiter].insert_or_assign(
                desc,
                core::str::common_prefix(symbols, delimiter));
            return it->second;
        }
    }

    std::size_t common_prefix_length(
        const google::protobuf::EnumDescriptor *desc,
        const std::string &delimiter)
    {
        return common_prefix(desc, delimiter).size();
    }

}  // namespace protobuf
