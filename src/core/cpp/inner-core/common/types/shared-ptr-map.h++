/// -*- c++ -*-
//==============================================================================
/// @file shared-ptr-map.h++
/// @brief Convenience map for `shared_ptr<>` values
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <map>
#include <memory>

namespace cc::core::types
{
    template <class K, class V, class MapType = std::map<K, std::shared_ptr<V>>>
    class shared_ptr_map : public MapType
    {
    public:
        using MapType::MapType;

        /// @brief
        ///    Return an existing mapped shared_ptr<> value or construct a new
        ///    one in place.
        /// @param[in] key
        ///    Mapping key.
        /// @param[in] args
        ///    Constructor args for the value, if it does not yet exist in the map.
        /// @returns
        ///    A `shared_ptr<>` reference to the existing or newvalue.
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

        /// @brief
        ///    Create a new `shared_ptr<>` reference and insert into the map.
        /// @param[in] key
        ///    Mapping key.
        /// @param[in] args
        ///    Constructor args for the value.
        /// @returns
        ///    A `shared_ptr<>` reference to the newly inserted value.
        template <class... Args>
        inline std::shared_ptr<V> &
        replace_shared(const K &key, Args &&...args)
        {
            auto [it, inserted] = this->insert_or_assign(
                key,
                std::make_shared<V>(args...));

            return it->second;
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
}  // namespace cc::core::types
