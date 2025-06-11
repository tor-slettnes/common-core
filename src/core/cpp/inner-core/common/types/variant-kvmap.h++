/// -*- c++ -*-
//==============================================================================
/// @file variant-kvmap.h++
/// @brief Key/value map of `Value` instances
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant-value.h++"
#include "string/convert.h++"
#include "types/streamable.h++"
#include "types/create-shared.h++"

namespace core::types
{
    class KeyValueMap : public std::unordered_map<Key, Value>,
                        public Streamable,
                        public enable_create_shared<KeyValueMap>
    {
        using Super = std::unordered_map<Key, Value>;

    public:
        using ptr = std::shared_ptr<KeyValueMap>;

    protected:
        using Super::Super;

    public:
        /// @brief
        ///   Obtain the value specified by `key`.
        /// @param[in] key
        ///   Mapping key
        /// @param[in] ignoreCase
        ///   If `true` and the exact key is not found, try again with a
        ///   case-insensitive search.
        /// @returns
        ///   The mapped value if found, otherwise an empty value.
        const Value &get(
            const std::string &key,
            const Value &fallback = {},
            bool ignoreCase = false) const noexcept;


        /// @brief
        ///   Obtain a optional container to the value specified by `key`, if found.
        /// @param[in] key
        ///   Mapping key
        /// @param[in] ignoreCase
        ///   If `true` and the exact key is not found, try again with a
        ///   case-insensitive search.
        /// @returns
        ///   The mapped value if found, otherwise an empty optional container.

        std::optional<Value> try_get(
            const std::string &key,
            bool ignoreCase = false) const noexcept;

        /// @brief
        ///   Obtain a pointer to the value specified by `key`, if found.
        /// @param[in] key
        ///   Mapping key
        /// @param[in] ignoreCase
        ///   If `true` and the exact key is not found, try again with a
        ///   case-insensitive search.
        /// @returns
        ///   Pointer to the mapepd value if found, otherwise `nullptr`.
        const Value *get_ptr(
            const std::string &key,
            bool ignoreCase = false) const noexcept;

        ValueListPtr get_valuelist(
            const std::string &key,
            bool ignoreCase = false) const noexcept;

        TaggedValueListPtr get_tvlist(
            const std::string &key,
            bool ignoreCase = false) const noexcept;

        KeyValueMapPtr get_kvmap(
            const std::string &key,
            bool ignoreCase = false) const noexcept;

        Value extract_value(const std::string &key,
                            const Value &fallback = {}) noexcept;

        std::vector<std::string> keys() const noexcept;
        ValueList values() const noexcept;
        TaggedValueList as_tvlist() const;
        KeyValueMap filtered() const noexcept;

        /// @brief
        ///   Replace items in this list with those from another map
        /// @param[in] other
        ///   KeyValueMap from which to import.
        /// @return
        ///   A reference to this updated instance.

        KeyValueMap &update(const KeyValueMap &other) noexcept;
        KeyValueMap &update(KeyValueMap &&other) noexcept;

        /// @brief
        ///   Merge values from another key value map, recursing
        ///   into sub-maps that exist in both places.
        /// @param[in] other
        ///   KeyValueMap from which to extract missing values.
        ///   These are removed from \p map.
        /// @return
        ///   A reference to this updated instance

        KeyValueMap &recursive_merge(KeyValueMap &other) noexcept;
        KeyValueMap &recursive_merge(KeyValueMap &&other) noexcept;

        /// @brief
        ///   Remove key/value pairs  that are identical to those
        ///   in \p basemap.
        /// @param[in] basemap
        ///   KeyValueMap against which to compare.  Keys present in this map
        ///   and with values identical to ours are removed.
        /// @return
        ///   A reference to this updated instance

        KeyValueMap &recursive_unmerge(const KeyValueMap &basemap) noexcept;

        /// @brief
        ///   Return the delta between the provided basemap and this instance
        //    as a new KeyValueMap reference.
        /// @param[in] basemap
        ///   Base map against which we are comparing
        /// @return
        ////    A new KeyValueMap instance.
        KeyValueMap recursive_delta(const KeyValueMap &basemap) const noexcept;

        /// @brief
        ///   Insert a pair into the map if a boolean condition is satisfied
        /// @param[in] condition
        ///   Whether to actually push or not
        /// @param[in] kv
        ///   Key/Value pair to insert
        /// @return
        ///   A pair comprising an iterator to the new value if inserted,
        ///   and a boolean indicating whether the value was inserted.
        std::pair<KeyValueMap::iterator, bool>
        insert_if(bool condition, const KeyValuePair &kv);

        /// @brief
        ///   Insert a key/value item into the map if a boolean condition is satisfied
        /// @param[in] condition
        ///   Whether to actually push or not
        /// @param[in] key
        ///   Mapping key. Any existing item with the same key is replaed
        /// @param[in] value
        ///   Valuie to insert
        /// @return
        ///   A pair comprising an iterator to the new value if inserted,
        ///   and a boolean indicating whether the value was inserted.
        std::pair<KeyValueMap::iterator, bool>
        insert_if(bool condition, const std::string &key, const Value &value);

        /// @brief
        ///   Insert a key/value item into the map if the value is non-empty
        /// @param[in] kv
        ///   Key/Value pair to insert
        /// @return
        ///   A pair comprising an iterator to the new value if inserted,
        ///   and a boolean indicating whether the value was inserted.
        std::pair<KeyValueMap::iterator, bool>
        insert_if_value(const KeyValuePair &kv);

        /// @brief
        ///   Insert a key/value item into the map if the value is non-empty
        /// @param[in] key
        ///   Mapping key. Any existing item with the same key is replaed
        /// @param[in] value
        ///   Valuie to insert
        /// @return
        ///   A pair comprising an iterator to the new value if inserted,
        ///   and a boolean indicating whether the value was inserted.
        std::pair<KeyValueMap::iterator, bool>
        insert_if_value(const std::string &key, const Value &value);

        template <class T>
        std::optional<T> try_get_as(const std::string &key, bool ignore_case = false) const
        {
            return this->get(key, {}, ignore_case).try_convert_to<T>();
        }

        /// @brief
        ///   Obtain key/value pairs where the value is a specific type

        template <class T>
        std::map<std::string, T> filter_by_type() const
        {
            std::map<std::string, T> result;
            for (const auto &item : *this)
            {
                if (const T *ptr = item.second.get_if<T>())
                {
                    result.insert_or_assign(item.first, *ptr);
                }
            }
            return result;
        }

        template <class T>
        static std::shared_ptr<KeyValueMap> create_shared_from(const T &obj)
        {
            auto kvmap = std::make_shared<KeyValueMap>();
            (*kvmap) << obj;
            return kvmap;
        }

        template <class T>
        static KeyValueMap create_from(const T &obj)
        {
            KeyValueMap kvmap;
            kvmap << obj;
            return kvmap;
        }

    public:
        void to_stream(std::ostream &stream) const override;
    };

    //--------------------------------------------------------------------------
    // Non-member functions

    template <class T>
    KeyValueMap &operator<<(KeyValueMap &kvmap, const T &inputs)
    {
        for (const auto &[key, value] : inputs)
        {
            kvmap[key] << value;
        }
        return kvmap;
    }

}  // namespace core::types
