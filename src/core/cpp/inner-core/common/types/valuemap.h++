/// -*- c++ -*-
//==============================================================================
/// @file valuemap.h++
/// @brief Map with non-throwing access method optimized for immutable values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <map>
#include <optional>
#include <stdexcept>

namespace core::types
{
    template <class K, class V, class MapType = std::map<K, V>>
    class ValueMap : public MapType
    {
    public:
        using MapType::MapType;

        inline const V &get(const K &key, const V &fallback = {}) const noexcept
        {
            try
            {
                return this->at(key);
            }
            catch (const std::out_of_range &)
            {
                return fallback;
            }
        }

        inline const V *get_ptr(const K &key) const noexcept
        {
            try
            {
                return &this->at(key);
            }
            catch (const std::out_of_range &)
            {
                return nullptr;
            }
        }

        inline std::optional<V> get_opt(const K &key) const noexcept
        {
            try
            {
                return this->at(key);
            }
            catch (const std::out_of_range &)
            {
                return {};
            }
        }
    };
}  // namespace core::types
