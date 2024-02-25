// -*- c++ -*-
//==============================================================================
/// @file variant-tvlist.c++
/// @brief Tag/Value list
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "variant-tvlist.h++"
#include "variant-list.h++"
#include "variant-kvmap.h++"
#include "status/exceptions.h++"

namespace shared::types
{
    TaggedValueList::const_iterator TaggedValueList::find(
        const Tag &tag,
        bool ignoreCase) const noexcept
    {
        if (tag.has_value())
        {
            std::string tagname = tag.value();
            std::string lowertag = str::tolower(tagname);
            for (auto it = this->cbegin(); it != this->cend(); it++)
            {
                if ((it->first.value_or("") == tagname) ||
                    (ignoreCase && (str::tolower(it->first.value_or("")) == lowertag)))
                {
                    return it;
                }
            }
        }
        return this->cend();
    }

    TaggedValueList::iterator TaggedValueList::find(
        const Tag &tag,
        bool ignoreCase) noexcept
    {
        if (tag.has_value())
        {
            std::string tagname = tag.value();
            std::string lowertag = str::tolower(tagname);
            for (auto it = this->begin(); it != this->end(); it++)
            {
                if ((it->first.value_or("") == tagname) ||
                    (ignoreCase && (str::tolower(it->first.value_or("")) == lowertag)))
                {
                    return it;
                }
            }
        }
        return this->end();
    }

    Value &TaggedValueList::at(const Tag &tag)
    {
        return const_cast<Value &>(const_cast<const TaggedValueList *>(this)->at(tag));
    }

    const Value &TaggedValueList::at(const Tag &tag) const
    {
        if (auto it = this->find(tag); it != this->end())
        {
            return it->second;
        }
        else
        {
            throw std::out_of_range(str::format("No such tag in TaggedValueList: %r", tag));
        }
    }

    Value &TaggedValueList::operator[](const Tag &tag) noexcept
    {
        try
        {
            return this->at(tag);
        }
        catch (const std::out_of_range &)
        {
            // auto lck = std::lock_guard(this->mtx);
            return this->emplace_back(tag, nullvalue).second;
        }
    }

    bool TaggedValueList::tagged() const noexcept
    {
        for (const auto &[t, v] : *this)
        {
            if (t.has_value())
            {
                return true;
            }
        }
        return false;
    }

    bool TaggedValueList::mappable() const noexcept
    {
        std::unordered_set<std::string> tags;

        for (const auto &[t, v] : *this)
        {
            if (t.has_value())
            {
                const std::string &tag = t.value();
                if (tags.count(tag))
                {
                    return false;
                }
                tags.insert(tag);
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    const Value &TaggedValueList::get(const Tag &tag,
                                      const Value &fallback,
                                      bool ignoreCase) const noexcept
    {
        // auto lck = std::lock_guard(this->mtx);
        if (auto it = this->find(tag, ignoreCase); it != this->end())
        {
            return it->second;
        }
        else
        {
            return fallback;
        }
    }

    const Value &TaggedValueList::get(uint index, const Value &fallback) const noexcept
    {
        try
        {
            return this->at(index).second;
        }
        catch (const std::out_of_range &)
        {
            return fallback;
        }
    }

    const Value &TaggedValueList::get(int index, const Value &fallback) const noexcept
    {
        try
        {
            return index >= 0
                       ? this->at(index).second
                       : this->at(this->size() + index).second;
        }
        catch (const std::out_of_range &)
        {
            return fallback;
        }
    }

    TagList TaggedValueList::tags() const noexcept
    {
        std::vector<Tag> tags;
        tags.reserve(this->size());
        for (const auto &[t, v] : *this)
        {
            tags.push_back(t);
        }
        return tags;
    }

    ValueList TaggedValueList::values() const noexcept
    {
        ValueList values;
        values.reserve(this->size());
        for (const auto &[t, v] : *this)
        {
            values.push_back(v);
        }
        return values;
    }

    /// Return a copy where items with empty tags are filtered out
    TaggedValueList TaggedValueList::filtered_tags() const noexcept
    {
        TaggedValueList filtered;

        filtered.reserve(this->size());
        for (const auto &tv : *this)
        {
            if (tv.first)
            {
                filtered.push_back(tv);
            }
        }
        filtered.shrink_to_fit();
        return filtered;
    }

    /// Return a copy where items with empty values are filtered out
    TaggedValueList TaggedValueList::filtered_values() const noexcept
    {
        TaggedValueList filtered;

        filtered.reserve(this->size());
        for (const auto &tv : *this)
        {
            if (tv.second)
            {
                filtered.push_back(tv);
            }
        }
        filtered.shrink_to_fit();
        return filtered;
    }

    KeyValueMap TaggedValueList::as_kvmap() const noexcept
    {
        KeyValueMap kvmap;

        for (const auto &[tag, value] : *this)
        {
            if (tag.has_value())
            {
                kvmap.emplace(tag.value(), value);
            }
        }
        return kvmap;
    }

    TaggedValueList &TaggedValueList::extend(const ValueList &vlist) noexcept
    {
        this->reserve(this->size() + vlist.size());
        for (const Value &value : vlist)
        {
            this->emplace_back(nulltag, value);
        }
        return *this;
    }

    TaggedValueList &TaggedValueList::extend(ValueList &&vlist) noexcept
    {
        this->reserve(this->size() + vlist.size());
        for (Value &value : vlist)
        {
            this->emplace_back(nulltag, std::move(value));
        }
        return *this;
    }

    TaggedValueList &TaggedValueList::extend(const TaggedValueList &other) noexcept
    {
        this->insert(this->end(), other.begin(), other.end());
        return *this;
    }

    TaggedValueList &TaggedValueList::extend(TaggedValueList &&other) noexcept
    {
        this->reserve(this->size() + other.size());
        for (auto it = other.begin(); it != other.end(); it++)
        {
            this->push_back(std::move(*it));
        }
        return *this;
    }

    TaggedValueList &TaggedValueList::update(const TaggedValueList &other) noexcept
    {
        for (auto input_it = other.begin(); input_it != other.end(); input_it++)
        {
            if (auto this_it = this->find(input_it->first); this_it != this->end())
            {
                *this_it = *input_it;
            }
            else
            {
                this->push_back(*input_it);
            }
        }
        return *this;
    }

    TaggedValueList &TaggedValueList::update(TaggedValueList &&other) noexcept
    {
        for (auto input_it = other.begin(); input_it != other.end(); input_it++)
        {
            if (auto this_it = this->find(input_it->first); this_it != this->end())
            {
                *this_it = std::move(*input_it);
            }
            else
            {
                this->push_back(std::move(*input_it));
            }
        }
        return *this;
    }

    TaggedValueList &TaggedValueList::merge(TaggedValueList &other) noexcept
    {
        // auto lck = std::lock_guard(this->mtx);
        this->reserve(this->size() + other.size());
        auto it = other.begin();
        while (it != other.end())
        {
            if (this->find(it->first) == this->end())
            {
                this->push_back(std::move(*it));
                it = other.erase(it);
            }
            else
            {
                it++;
            }
        }
        this->shrink_to_fit();
        return *this;
    }

    TaggedValueList &TaggedValueList::merge(TaggedValueList &&other) noexcept
    {
        return this->merge(other);
    }

    bool TaggedValueList::push_if(bool condition, const TaggedValue &tv)
    {
        if (condition)
        {
            this->push_back(tv);
        }
        return condition;
    }

    bool TaggedValueList::push_if(bool condition, const Value &value)
    {
        if (condition)
        {
            this->push_back({{}, value});
        }
        return condition;
    }

    void TaggedValueList::to_stream(std::ostream &stream) const
    {
        this->to_stream(stream, "{", ", ", "}");
    }

    void TaggedValueList::to_stream(std::ostream &stream,
                                    const std::string &prefix,
                                    const std::string &infix,
                                    const std::string &postfix) const
    {
        // auto lck = std::lock_guard(this->mtx);
        stream << prefix;
        bool comma = false;
        for (const auto &[tag, value] : *this)
        {
            if (comma)
            {
                stream << infix;
            }
            else
            {
                comma = true;
            }
            if (tag.has_value())
            {
                stream << tag.value() << "=";
            }
            value.to_literal_stream(stream);
        }
        stream << postfix;
    }

}  // namespace shared::types
