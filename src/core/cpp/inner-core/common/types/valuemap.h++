/// -*- c++ -*-
//==============================================================================
/// @file valuemap.h++
/// @brief Map with non-throwing access method optimized for immutable values
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <map>
#include <optional>
#include <stdexcept>
#include <vector>

namespace core::types
{
    template <class K, class V, class MapType = std::map<K, V>>
    class ValueMap : public MapType
    {
    public:
        using MapType::MapType;

        const V &get(const K &key, const V &fallback = {}) const noexcept
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

        const V *get_ptr(const K &key) const noexcept
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

        V *get_ptr(const K &key) noexcept
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

        std::optional<V> get_opt(const K &key) const noexcept
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

        std::vector<K> keys() const noexcept
        {
            std::vector<K> result;
            result.reserve(this->size());
            for (const auto &[key, value]: *this)
            {
                result.push_back(key);
            }
            return result;
        }

        std::vector<V> values() const noexcept
        {
            std::vector<V> result;
            result.reserve(this->size());
            for (const auto &[key, value]: *this)
            {
                result.push_back(value);
            }
            return result;
        }
    };
}  // namespace core::types
