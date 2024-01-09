/// -*- c++ -*-
//==============================================================================
/// @file symbolmap.h++
/// @brief Unordered string map with symbol lookup
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/misc.h++"

#include <optional>
#include <iostream>
#include <string>
#include <map>

namespace cc::types
{
    template <class KeyType,
              class MapType = std::map<KeyType, std::string>>
    class SymbolMap : public MapType
    {
    public:
        using MapType::MapType;

        inline std::optional<std::string> to_string(const KeyType &key) const noexcept
        {
            if (auto it = this->find(key); it != this->end())
            {
                return it->second;
            }
            else
            {
                return {};
            }
        }

        inline std::string to_string(const KeyType &key, const std::string &fallback) const noexcept
        {
            return to_string(key).value_or(fallback);
        }

        inline std::optional<KeyType> from_string(const std::string &symbol) const noexcept
        {
            std::string lower_symbol = str::tolower(symbol);
            for (const auto &[candidate_key, candidate_symbol] : *this)
            {
                if (str::tolower(candidate_symbol) == lower_symbol)
                {
                    return candidate_key;
                }
            }
            return {};
        }

        inline KeyType from_string(const std::string &symbol, const KeyType &fallback) const noexcept
        {
            return from_string(symbol).value_or(fallback);
        }

        inline std::ostream &to_stream(std::ostream &stream,
                                       const KeyType &key,
                                       const std::optional<std::string> &fallback = {}) const
        {
            if (const std::optional<std::string> &opt_symbol = this->to_string(key))
            {
                stream << *opt_symbol;
            }
            else if (fallback)
            {
                stream << *fallback;
            }
            else
            {
                stream.setstate(std::ios_base::failbit);
            }
            return stream;
        }

        inline std::istream &from_stream(std::istream &stream,
                                         KeyType *key,
                                         const std::optional<KeyType> &fallback = {},
                                         bool flag_unknown = true) const
        {
            std::string symbol;
            stream >> symbol;
            str::tolower(symbol);
            if (const std::optional<KeyType> &opt_key = this->from_string(symbol))
            {
                *key = *opt_key;
            }
            else
            {
                if (fallback)
                {
                    *key = *fallback;
                }

                if (flag_unknown)
                {
                    stream.setstate(std::ios_base::failbit);
                }
            }
            return stream;
        }

        inline void join_keys(std::ostream &stream,
                              const std::string &separator = "|",
                              const std::string &prefix = "{",
                              const std::string &suffix = "}") const
        {
            stream << prefix;
            bool print_sep = false;
            for (const auto &[k, v] : *this)
            {
                if (print_sep)
                {
                    stream << separator;
                }
                stream << k;
                print_sep = true;
            }
            stream << suffix;
        }

        inline std::string joined_keys(const std::string &separator = "|",
                                       const std::string &prefix = "{",
                                       const std::string &suffix = "}") const
        {
            std::stringstream s;
            this->join_keys(s, separator, prefix, suffix);
            return s.str();
        }

        inline std::vector<KeyType> keys() const
        {
            std::vector<std::string> keys;
            keys.reserve(this->size());
            for (const auto &[k, v] : *this)
            {
                keys.push_back(k);
            }
            return keys;
        }

        inline std::vector<std::string> symbols() const
        {
            std::vector<std::string> symbols;
            symbols.reserve(this->size());
            for (const auto &[k, v] : *this)
            {
                symbols.push_back(v);
            }
            return symbols;
        }

        inline std::string joined_symbols(const std::string &separator = "|",
                                          const std::string &prefix = "{",
                                          const std::string &suffix = "}") const
        {
            return prefix + str::join(this->symbols(), separator) + suffix;
        }
    };
}  // namespace cc::types
