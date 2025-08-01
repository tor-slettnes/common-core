/// -*- c++ -*-
//==============================================================================
/// @file glib-variant.h++
/// @brief Wrappers/containers for Glib::VariantBase
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "platform/symbols.h++"

#include <glibmm/variant.h>

#define URI_PREFIX_FILE "file://"s

namespace core::glib
{
    using VariantMap = std::map<std::string, Glib::VariantBase>;
    using VariantMaps = std::vector<VariantMap>;
    using SettingsMap = std::map<std::string, VariantMap>;
    using SettingsContainer = Glib::Variant<SettingsMap>;

    //==========================================================================
    /// @brief convert from Glib::Variant to specific scalar, non-enumerated data type

    /// convert from Glib::Variant to specific scalar, non-enumerated data type
    template <class T, std::enable_if_t<!std::is_enum_v<T>, bool> = true>
    inline bool variant_cast(const Glib::VariantBase& obj, T* value)
    {
        bool cast = false;
        if (obj)
        {
            try
            {
                *value = Glib::VariantBase::cast_dynamic<Glib::Variant<T>>(obj).get();
                cast = true;
            }
            catch (const std::bad_cast& e)
            {
                logf_warning("VariantBase %r (type %r) cannot be converted to %s: %s",
                             obj,
                             obj.get_type_string(),
                             TYPE_NAME_FULL(T),
                             e.what());
            }
        }

        return cast;
    }

    /// Convert from Glib::Variant value to enumerated data type
    template <class T, std::enable_if_t<std::is_enum_v<T>, bool> = true>
    inline bool variant_cast(const Glib::VariantBase& obj, T* value)
    {
        uint scalar;
        bool cast = variant_cast<uint>(obj, &scalar);
        if (cast)
        {
            *value = static_cast<T>(scalar);
        }
        return cast;
    }

    /// Convert from Glib::Variant value to data type
    template <class T>
    inline T variant_cast(const Glib::VariantBase& obj)
    {
        T value = {};
        variant_cast<T>(obj, &value);
        return value;
    }

    /// Convert one element of a Glib::VariantContainer object to specific data type
    template <class T>
    bool variant_cast(const Glib::VariantContainerBase& obj,
                      uint index,
                      T* value)
    {
        if (index < obj.get_n_children())
        {
            Glib::VariantBase child;
            obj.get_child(child, index);
            return variant_cast<T>(child, value);
        }
        else
        {
            logf_warning(
                "VariantContainerBase object %r (type %r) "
                "has only %d children, could not get #%d.",
                obj,
                obj.get_type_string(),
                obj.get_n_children(),
                index);
            return false;
        }
    }

    /// Convert one element of a Glib::VariantContainer object to specific data type
    template <class T>
    inline T variant_cast(const Glib::VariantContainerBase& obj, uint index)
    {
        T value = {};
        variant_cast<T>(obj, index, &value);
        return value;
    }

    //==========================================================================
    /// @brief Extract values from VariantBase maps

    template <class T>
    inline bool extract_value(const VariantMap& map,
                              const std::string& key,
                              T* value)
    {
        auto it = map.find(key);
        return (it != map.end()) && variant_cast(it->second, value);
    }

    template <class T>
    inline bool extract_mapped(const VariantMap& map,
                               const core::types::SymbolMap<T>& lookup_map,
                               const std::string& key,
                               T* value)
    {
        std::string s;
        if (extract_value(map, key, &s))
        {
            if (const std::optional<T>& opt_value = lookup_map.try_from_string(s))
            {
                *value = *opt_value;
                return true;
            }
        }
        return false;
    }

    inline bool extract_filepath(const VariantMap& map,
                                 const std::string& key,
                                 std::filesystem::path* value)
    {
        std::string s;
        bool is_path = (extract_value(map, key, &s) &&
                        core::str::startswith(s, URI_PREFIX_FILE));
        if (is_path)
        {
            *value = s.substr(URI_PREFIX_FILE.length());
        }
        return is_path;
    }

    //======================================================================
    // Template methods to insert into Network Manager settings maps

    inline void insert_value(VariantMap* map,
                             const std::string& key,
                             const std::string& value)
    {
        if (!value.empty())
        {
            map->emplace(key, Glib::Variant<Glib::ustring>::create(value));
        }
    }

    template <class T>
    inline void insert_value(VariantMap* map,
                             const std::string& key,
                             const T& value)
    {
        const static T empty = {};
        if (value != empty)
        {
            map->emplace(key, Glib::Variant<T>::create(value));
        }
    }

    template <class T>
    inline void insert_mapped(VariantMap* map,
                              const std::map<T, std::string> lookup_map,
                              const std::string& key,
                              const T& value)
    {
        auto it = lookup_map.find(value);
        if (it != lookup_map.end())
        {
            insert_value(map, key, it->second);
        }
    }
}  // namespace core::glib

/// Output stream support for Glib VariantBase, for use in log output.
namespace Glib
{
    std::ostream& operator<<(std::ostream& stream, const VariantBase& obj);
}  // namespace Glib
