/// -*- c++ -*-
//==============================================================================
/// @file expand.h++
/// @brief String formatting from template string, using C++ output streams
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <optional>
#include <unordered_map>
#include <regex>
#include <string>
#include <sstream>
#include <stdexcept>

namespace core::str
{
    /// @brief
    ///     Expands brace-enclosed keys wihtin to string to corresponding values from map.
    /// @tparam V
    ///     Value type. Must support output stream "<<" operator.
    /// @param[in] format
    ///     String containing zero or more brace-enclosed keys of the form "{key}".
    /// @param[in] kvmap
    ///     Key/value map.
    /// @param[in] fallback
    ///     Value to insert for keys not found in map.  If unset, the original key is
    ///     inserted without modification.  Note that the default value is an empty
    ///     string; use the literal `{}` to indicate an empty value.
    /// @return
    ///     A copy of the format string where "{key}" occurences have been
    ///     replaced by their corresponding values from the map.
    /// @note
    ///     Keys that do not exist in the map are replaced by an empty string.

    template <class V = std::string, class MapType = std::unordered_map<std::string, V>>
    std::string expand(const std::string &format,
                       const MapType &kvmap,
                       const std::optional<V> &fallback = "")
    {
        static const std::regex rx("\\{(\\w+)\\}");

        std::stringstream ss;
        auto rx_begin = std::sregex_iterator(format.begin(), format.end(), rx);
        auto rx_end = std::sregex_iterator();
        std::size_t pos = 0, next = 0;
        for (auto matchit = rx_begin; matchit != rx_end; matchit++)
        {
            pos = matchit->position(0);
            ss << format.substr(next, pos - next);
            try
            {
                ss << kvmap.at(matchit->str(1));
            }
            catch (const std::out_of_range &)
            {
                ss << fallback.value_or(matchit->str(0));
            }
            next = pos + matchit->length(0);
        }
        ss << format.substr(next);
        return ss.str();
    }
}  // namespace core::str
