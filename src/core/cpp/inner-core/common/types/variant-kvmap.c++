// -*- c++ -*-
//==============================================================================
/// @file variant-kvmap.c++
/// @brief Key/value map of `Value` instances
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "variant-kvmap.h++"
#include "variant-list.h++"
#include "variant-tvlist.h++"
#include "string/misc.h++"

namespace cc::types
{
    const Value &KeyValueMap::get(const std::string &key,
                                  const Value &fallback,
                                  bool ignoreCase) const noexcept
    {
        try
        {
            // First a plain lookup
            return this->at(key);
        }
        catch (const std::out_of_range &)
        {
            // Then, try a case insensitive search (if selected)
            if (ignoreCase)
            {
                std::string lowerkey = str::tolower(key);
                for (const auto &[c_key, c_value] : *this)
                {
                    if (str::tolower(c_key) == lowerkey)
                    {
                        return c_value;
                    }
                }
            }
            // Nope, return fallback.
            return fallback;
        }
    }

    TaggedValueList KeyValueMap::as_tvlist() const
    {
        return TaggedValueList(this->cbegin(), this->cend());
    }

    std::vector<std::string> KeyValueMap::keys() const noexcept
    {
        std::vector<std::string> keys;
        keys.reserve(this->size());
        for (const auto &[k, v] : *this)
        {
            keys.push_back(k);
        }
        return keys;
    }

    ValueList KeyValueMap::values() const noexcept
    {
        ValueList values;
        for (const auto &[k, v] : *this)
        {
            values.push_back(v);
        }
        return values;
    }

    /// Return a copy where items with empty values are filtered out
    KeyValueMap KeyValueMap::filtered() const noexcept
    {
        KeyValueMap filtered;

        for (const auto &kv : *this)
        {
            if (kv.second)
            {
                filtered.insert(kv);
            }
        }
        return filtered;
    }

    KeyValueMap &KeyValueMap::update(const KeyValueMap &other) noexcept
    {
        for (const auto &[key, value] : other)
        {
            this->insert_or_assign(key, value);
        }
        return *this;
    }

    KeyValueMap &KeyValueMap::update(KeyValueMap &&other) noexcept
    {
        this->swap(other);
        this->merge(other);
        return *this;
    }

    KeyValueMap &KeyValueMap::recursive_merge(KeyValueMap &other) noexcept
    {
        for (auto &[key, other_value] : other)
        {
            if (auto this_it = this->find(key); this_it != this->end())
            {
                KeyValueMapRef *this_map = std::get_if<KeyValueMapRef>(&this_it->second);
                KeyValueMapRef *other_map = std::get_if<KeyValueMapRef>(&other_value);
                if (this_map && other_map)
                {
                    (*this_map)->recursive_merge(**other_map);
                }
            }
            else
            {
                this->insert_or_assign(key, std::move(other_value));
            }
        }
        return *this;
    }

    KeyValueMap &KeyValueMap::recursive_merge(KeyValueMap &&other) noexcept
    {
        return this->recursive_merge(other);
    }

    KeyValueMap &KeyValueMap::recursive_unmerge(const KeyValueMap &basemap) noexcept
    {
        for (auto base_it = basemap.begin(); base_it != basemap.end(); base_it++)
        {
            if (auto this_it = this->find(base_it->first); this_it != this->end())
            {
                KeyValueMapRef *this_ptr = std::get_if<KeyValueMapRef>(&this_it->second);
                const KeyValueMapRef *base_ptr = std::get_if<KeyValueMapRef>(&base_it->second);
                if (this_ptr && base_ptr)
                {
                    (*this_ptr)->recursive_unmerge(**base_ptr);
                }
                else if (this_it->second == base_it->second)
                {
                    this->erase(this_it);
                }
            }
        }
        return *this;
    }

    KeyValueMap KeyValueMap::recursive_delta(const KeyValueMap &basemap) const noexcept
    {
        KeyValueMap delta;
        for (auto this_it = this->begin(); this_it != this->end(); this_it++)
        {
            const std::string &key = this_it->first;
            Value value;

            if (auto base_it = basemap.find(key); base_it != basemap.end())
            {
                const KeyValueMapRef *this_ptr = std::get_if<KeyValueMapRef>(&this_it->second);
                const KeyValueMapRef *base_ptr = std::get_if<KeyValueMapRef>(&base_it->second);
                if (this_ptr && base_ptr)
                {
                    auto submap = KeyValueMap::create_shared((*this_ptr)->recursive_delta(**base_ptr));
                    if (!submap->empty())
                    {
                        value = submap;
                    }
                }
                else if (this_it->second != base_it->second)
                {
                    value = this_it->second;
                }
            }
            else
            {
                value = this_it->second;
            }

            if (value)
            {
                delta.insert_or_assign(key, value);
            }
        }

        return delta;
    }

    std::pair<KeyValueMap::iterator, bool> KeyValueMap::insert_if(
        bool condition,
        const KeyValuePair &kv)
    {
        return this->insert_if(condition, kv.first, kv.second);
    }

    std::pair<KeyValueMap::iterator, bool> KeyValueMap::insert_if(
        bool condition,
        const std::string &key,
        const Value &value)
    {
        if (condition)
        {
            return this->insert_or_assign(key, value);
        }
        else
        {
            return {this->end(), false};
        }
    }

    void KeyValueMap::to_stream(std::ostream &stream) const
    {
        bool comma = false;
        stream << "{";
        for (const auto &[key, value] : *this)
        {
            if (comma)
            {
                stream << ", ";
            }
            else
            {
                comma = true;
            }
            stream << std::quoted(key) << ": ";
            value.to_literal_stream(stream);
        }
        stream << "}";
    }
}  // namespace cc::types
