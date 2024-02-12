/// -*- c++ -*-
//==============================================================================
/// @file variant-kvmap.h++
/// @brief Key/value map of `Value` instances
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "variant-value.h++"
#include "types/streamable.h++"
#include "types/create-shared.h++"

namespace shared::types
{
    class KeyValueMap : public std::map<Key, Value>,
                        public Streamable,
                        public enable_create_shared<KeyValueMap>
    {
        using Super = std::map<Key, Value>;

    protected:
        using Super::Super;

    public:
        const Value &get(const std::string &key,
                         const Value &fallback = {},
                         bool ignoreCase = false) const noexcept;

        std::vector<std::string> keys() const noexcept;
        ValueList values() const noexcept;
        TaggedValueList as_tvlist() const;
        KeyValueMap filtered() const noexcept;

        /// \brief
        ///    Replace items in this list with those from another map
        /// \param[in] kvmap
        ///    KeyValueMap from which to import.
        /// \return
        ///     A reference to this updated instance.

        KeyValueMap &update(const KeyValueMap &other) noexcept;
        KeyValueMap &update(KeyValueMap &&other) noexcept;

        /// \brief
        ///    Merge values from another key value map, recursing
        ///    into sub-maps that exist in both places.
        /// \param[in] kvmap
        ///    KeyValueMap from which to extract missing values.
        ///    These are removed from \p map.
        /// \return
        ///    A reference to this updated instance

        KeyValueMap &recursive_merge(KeyValueMap &other) noexcept;
        KeyValueMap &recursive_merge(KeyValueMap &&other) noexcept;

        /// \fn recursive_unmerge
        /// \brief
        ///    Remove key/value pairs  that are identical to those
        ///    in \p basemap.
        /// \param[in] basemap
        ///    KeyValueMap against which to compare.  Keys present in this map
        ///    and with values identical to ours are removed.
        /// \return
        ///    A reference to this updated instance

        KeyValueMap &recursive_unmerge(const KeyValueMap &basemap) noexcept;

        /// \fn recursive_delta
        /// \brief
        ///     Return the delta between the provided basemap and this instance
        //      as a new KeyValueMap reference.
        /// \param[in] basemap
        ///     Base map against which we are comparing
        /// \return
        ////    A new KeyValueMap instance.
        KeyValueMap recursive_delta(const KeyValueMap &basemap) const noexcept;

        /// \brief
        ///    Insert a pair into them map if a boolean condition is satisfied
        /// \param[in] condition
        ///    Whether to actually push or not
        /// \param[in] element
        ///    Key/Value pair to insert
        std::pair<KeyValueMap::iterator, bool>
        insert_if(bool condition, const KeyValuePair &kv);

        std::pair<KeyValueMap::iterator, bool>
        insert_if(bool condition, const std::string &key, const Value &value);

    public:
        void to_stream(std::ostream &stream) const override;
    };

}  // namespace shared::types
