/// -*- c++ -*-
//==============================================================================
/// @file shared_ptr_map.h++
/// @brief Convenience map for `shared_ptr<>` values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <map>
#include <memory>

namespace core::types
{
    template <class K, class V, class MapType = std::map<K, std::shared_ptr<V>>>
    class shared_ptr_map : public MapType
    {
    public:
        using MapType::MapType;

        template <class... Args>
        inline std::shared_ptr<V> &
        emplace_shared(const K &key, Args &&...args)
        {
            std::shared_ptr<V> &item = (*this)[key];
            if (!item)
            {
                item = std::make_shared<V>(args...);
            }
            return item;
        }

        inline std::shared_ptr<V>
        get(const K &key, const std::shared_ptr<V> &fallback = {}) const noexcept
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
    };
}  // namespace core::types
