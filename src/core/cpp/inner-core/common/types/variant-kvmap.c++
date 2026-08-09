// -*- c++ -*-
//==============================================================================
/// @file variant-kvmap.c++
/// @brief Key/value map of `Value` instances
/// @author Tor Slettnes
//==============================================================================

#include "variant-kvmap.h++"
#include "variant-list.h++"
#include "variant-tvlist.h++"
#include "string/misc.h++"

namespace cc::core::types
{
    Value KeyValueMap::get(
        const std::string& key,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get(key, ignoreCase).value_or(fallback);
    }

    Value KeyValueMap::get_nonempty(
        const std::string& key,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get_nonempty(key, ignoreCase).value_or(fallback);
    }

    Value KeyValueMap::get_any_of(
        const std::vector<std::string>& candidates,
        const Value& fallback,
        bool ignoreCase) const noexcept
    {
        return this->try_get_any_of(candidates, ignoreCase).value_or(fallback);
    }

    Value KeyValueMap::get_nested(
            const std::vector<std::string> &path,
            const Value &fallback,
            bool ignoreCase) const noexcept
    {
        return this->try_get_nested(path, ignoreCase).value_or(fallback);
    }

    ValueListPtr KeyValueMap::get_valuelist_ptr(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        if (const auto &value = this->try_get(key, ignoreCase))
        {
            return value->get_valuelist_ptr();
        }
        else
        {
            return {};
        }
    }

    TaggedValueListPtr KeyValueMap::get_tvlist_ptr(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        if (const auto &value = this->try_get(key, ignoreCase))
        {
            return value->get_tvlist_ptr();
        }
        else
        {
            return {};
        }
    }

    KeyValueMapPtr KeyValueMap::get_kvmap_ptr(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        if (const auto &value = this->try_get(key, ignoreCase))
        {
            return value->get_kvmap_ptr();
        }
        else
        {
            return {};
        }
    }

    std::optional<Value> KeyValueMap::try_get(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        try
        {
            return this->at(key);
        }
        catch (const std::out_of_range&)
        {
            // Then, try a case insensitive search (if selected)
            if (ignoreCase)
            {
                std::string lowerkey = str::tolower(key);
                for (const auto& [c_key, c_value] : *this)
                {
                    if (str::tolower(c_key) == lowerkey)
                    {
                        return c_value;
                    }
                }
            }
            // Nope, nothing found.
            return {};
        }
    }

    std::optional<Value> KeyValueMap::try_get_nested(
        const std::vector<std::string>& path,
        bool ignoreCase) const noexcept
    {
        Value value = KeyValueMap::create_shared(*this);
        for (const std::string& element : path)
        {
            if (const auto &next = value.try_get(element))
            {
                value = next.value();
            }
            else
            {
                return {};
            }
        }
        return value;
    }


    std::optional<Value> KeyValueMap::try_get_nonempty(
        const std::string& key,
        bool ignoreCase) const noexcept
    {
        if (const auto &value = this->try_get(key, ignoreCase))
        {
            if (value->has_nonempty_value())
            {
                return value;
            }
        }
        return {};
    }

    std::optional<Value> KeyValueMap::try_get_any_of(
        const std::vector<std::string>& candidates,
        bool ignoreCase) const noexcept
    {
        for (const std::string& candidate : candidates)
        {
            if (const auto &value = this->try_get(candidate, ignoreCase))
            {
                return value;
            }
        }
        return {};
    }

    Value KeyValueMap::extract_value(const std::string& key,
                                     const Value& fallback) noexcept
    {
        if (auto nh = this->extract(key))
        {
            return nh.mapped();
        }
        else
        {
            return fallback;
        }
    }

    TaggedValueList KeyValueMap::as_tvlist() const noexcept
    {
        return TaggedValueList(this->cbegin(), this->cend());
    }

    TaggedValueListPtr KeyValueMap::as_tvlist_ptr() const noexcept
    {
        return std::make_shared<TaggedValueList>(this->cbegin(), this->cend());
    }

    std::vector<std::string> KeyValueMap::keys() const noexcept
    {
        std::vector<std::string> keys;
        keys.reserve(this->size());
        for (const auto& [k, v] : *this)
        {
            keys.push_back(k);
        }
        return keys;
    }

    ValueList KeyValueMap::values() const noexcept
    {
        return *this->values_ptr();
    }

    ValueListPtr KeyValueMap::values_ptr() const noexcept
    {
        auto values = std::make_shared<ValueList>();
        values->reserve(this->size());
        for (const auto& [k, v] : *this)
        {
            values->push_back(v);
        }
        return values;
    }

    /// Return a copy where items with empty values are filtered out
    KeyValueMap KeyValueMap::filtered() const noexcept
    {
        KeyValueMap filtered;

        for (const auto& kv : *this)
        {
            if (kv.second.has_nonempty_value())
            {
                filtered.insert(kv);
            }
        }
        return filtered;
    }

    KeyValueMap KeyValueMap::flattened() const noexcept
    {
        KeyValueMap flattened;
        for (const auto& [key, value] : *this)
        {
            switch (value.type())
            {
            case ValueType::KVMAP:
                flattened.merge(value.get_kvmap().flattened());

            case ValueType::TVLIST:
                flattened.merge(value.as_kvmap().flattened());
                break;

            case ValueType::VALUELIST:
                if (value.get_valuelist().size() > 0)
                {
                    flattened.insert_or_assign(key, value.front());
                }
                break;

            default:
                flattened.insert_or_assign(key, value);
                break;
            }
        }
        return flattened;
    }

    KeyValueMap KeyValueMap::deepcopy() const noexcept
    {
        KeyValueMap copy;
        for (const auto& [key, value] : *this)
        {
            copy.insert_or_assign(key, value.deepcopy());
        }
        return copy;
    }

    KeyValueMap& KeyValueMap::update(const KeyValueMap& other) noexcept
    {
        for (const auto& [key, value] : other)
        {
            this->insert_or_assign(key, value);
        }
        return *this;
    }

    KeyValueMap& KeyValueMap::update(KeyValueMap&& other) noexcept
    {
        this->swap(other);
        this->merge(other);
        return *this;
    }

    KeyValueMap& KeyValueMap::recursive_merge(const KeyValueMap& other) noexcept
    {
        for (const auto& [key, value] : other)
        {
            if (auto this_map = this->get_kvmap_ptr(key))
            {
                this_map->recursive_merge(value.as_kvmap());
            }
            else
            {
                this->try_emplace(key, value);
            }
        }
        return *this;
    }

    KeyValueMap& KeyValueMap::recursive_merge(KeyValueMap&& other) noexcept
    {
        while (!other.empty())
        {
            auto nh = other.extract(other.begin());
            if (auto this_map = this->get_kvmap_ptr(nh.key()))
            {
                if (auto other_map = nh.mapped().get_kvmap_ptr())
                {
                    this_map->recursive_merge(std::move(*other_map));
                }
            }
            else
            {
                this->insert(std::move(nh));
            }
        }
        return *this;
    }

    KeyValueMap& KeyValueMap::recursive_unmerge(const KeyValueMap& basemap) noexcept
    {
        for (auto base_it = basemap.begin(); base_it != basemap.end(); base_it++)
        {
            if (auto this_it = this->find(base_it->first); this_it != this->end())
            {
                KeyValueMapPtr* this_ptr = std::get_if<KeyValueMapPtr>(&this_it->second);
                const KeyValueMapPtr* base_ptr = std::get_if<KeyValueMapPtr>(&base_it->second);
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

    KeyValueMap KeyValueMap::recursive_delta(const KeyValueMap& basemap) const noexcept
    {
        KeyValueMap delta;
        for (auto this_it = this->begin(); this_it != this->end(); this_it++)
        {
            const std::string& key = this_it->first;
            Value value;

            if (auto base_it = basemap.find(key); base_it != basemap.end())
            {
                const KeyValueMapPtr* this_ptr = std::get_if<KeyValueMapPtr>(&this_it->second);
                const KeyValueMapPtr* base_ptr = std::get_if<KeyValueMapPtr>(&base_it->second);
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
        const KeyValuePair& kv)
    {
        return this->insert_if(condition, kv.first, kv.second);
    }

    std::pair<KeyValueMap::iterator, bool> KeyValueMap::insert_if(
        bool condition,
        const std::string& key,
        const Value& value)
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

    std::pair<KeyValueMap::iterator, bool>
    KeyValueMap::insert_if_value(const KeyValuePair& kv)
    {
        return this->insert_if_value(kv.first, kv.second);
    }

    std::pair<KeyValueMap::iterator, bool>
    KeyValueMap::insert_if_value(const std::string& key, const Value& value)
    {
        if (value.has_nonempty_value())
        {
            return this->insert_or_assign(key, value);
        }
        else
        {
            return {this->end(), false};
        }
    }

    void KeyValueMap::to_stream(std::ostream& stream) const
    {
        bool comma = false;
        stream << "{";
        for (const auto& [key, value] : *this)
        {
            if (comma)
            {
                stream << ", ";
            }
            else
            {
                comma = true;
            }
            stream << key << ": ";
            value.to_literal_stream(stream);
        }
        stream << "}";
    }

}  // namespace cc::core::types
