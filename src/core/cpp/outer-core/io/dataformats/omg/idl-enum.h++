/// -*- c++ -*-
//==============================================================================
/// @file idl-enum.h++
/// @brief Misc. utility functions for IDL enums
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/symbolmap.h++"

#include <rti/topic/TopicTraits.hpp>

#include <optional>
#include <type_traits>

namespace idl
{
    template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    core::types::SymbolMap<T> enum_symbols()
    {
        core::types::SymbolMap<T> map;
        auto tc = rti::topic::dynamic_type<T>::get();

        for (const auto &member : tc.members())
        {
            map.insert_or_assign(static_cast<T>(member.ordinal()), member.name());
        }
        return map;
    }

    template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    std::optional<T> enum_value(const std::string &name)
    {
        try
        {
            auto tc = rti::topic::dynamic_type<T>::get();
            return static_cast<T>(tc.member(name).ordinal());
        }
        catch (const dds::core::PreconditionNotMetError &)
        {
            return {};
        }
    }

    template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    std::optional<std::string> enum_name(T value)
    {
        try
        {
            auto tc = rti::topic::dynamic_type<T>::get();
            return tc.member(tc.find_member_by_ordinal(static_cast<int32_t>(value))).name();
        }
        catch (const dds::core::PreconditionNotMetError &)
        {
            return {};
        }
    }

    template <class T>
    std::optional<std::string> enum_name(uint32_t value)
    {
        try
        {
            auto tc = rti::topic::dynamic_type<T>::get();
            return tc.member(tc.find_member_by_ordinal(value)).name();
        }
        catch (const dds::core::PreconditionNotMetError &)
        {
            return {};
        }
    }

}  // namespace idl
