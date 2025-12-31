// -*- c++ -*-
//==============================================================================
/// @file variant-list.c++
/// @brief Vector of `Value` instances
/// @author Tor Slettnes
//==============================================================================

#include "variant-list.h++"

namespace core::types
{
    const Value &ValueList::front(const Value &fallback) const noexcept
    {
        if (!this->empty())
        {
            return Super::front();
        }
        else
        {
            return fallback;
        }
    }

    const Value &ValueList::back(const Value &fallback) const noexcept
    {
        if (!this->empty())
        {
            return Super::back();
        }
        else
        {
            return fallback;
        }
    }

    const Value &ValueList::get(uint index, const Value &fallback) const noexcept
    {
        try
        {
            return this->at(index);
        }
        catch (const std::out_of_range &)
        {
            return fallback;
        }

    }

    const Value &ValueList::get(int index, const Value &fallback) const noexcept
    {
        try
        {
            return index >= 0
                       ? this->at(index)
                       : this->at(this->size() + index);
        }
        catch (const std::out_of_range &)
        {
            return fallback;
        }
    }

    std::optional<Value> ValueList::try_get(uint index) const noexcept
    {
        try
        {
            return this->at(index);
        }
        catch (const std::out_of_range &)
        {
            return {};
        }
    }

    std::optional<Value> ValueList::try_get(int index) const noexcept
    {
        try
        {
            return index >= 0
                       ? this->at(index)
                       : this->at(this->size() + index);
        }
        catch (const std::out_of_range &)
        {
            return {};
        }
    }

    TaggedValueList ValueList::as_tvlist() const noexcept
    {
        TaggedValueList tvlist;
        tvlist.reserve(this->size());
        for (const auto &value : *this)
        {
            tvlist.push_back({nulltag, value});
        }
        return tvlist;
    }

    ValueList ValueList::filtered_values() const noexcept
    {
        ValueList filtered;
        filtered.reserve(this->size());

        for (const auto &value : *this)
        {
            if (value.has_nonempty_value())
            {
                filtered.push_back(value);
            }
        }

        filtered.shrink_to_fit();
        return filtered;
    }

    ValueList &ValueList::extend(const ValueList &other)
    {
        this->insert(this->end(), other.begin(), other.end());
        return *this;
    }

    ValueList &ValueList::extend(ValueList &&other)
    {
        this->reserve(this->size() + other.size());
        for (auto it = other.begin(); it != other.end(); it++)
        {
            this->push_back(std::move(*it));
        }
        return *this;
    }

    ValueList::iterator ValueList::append(const Value &value)
    {
        return this->insert(this->end(), value);
    }

    ValueList::iterator ValueList::append(Value &&value)
    {
        return this->insert(this->end(), std::move(value));
    }

    ValueList::AppendResult ValueList::append_if(bool condition, const Value &value)
    {
        return {
            condition ? this->insert(this->end(), value) : this->end(),
            condition,
        };
    }

    ValueList::AppendResult ValueList::append_if(bool condition, Value &&value)
    {
        return {
            condition ? this->insert(this->end(), std::move(value)) : this->end(),
            condition,
        };
    }

    ValueList::AppendResult ValueList::append_if_value(const Value &value)
    {
        return this->append_if(value.has_nonempty_value(), value);
    }

    ValueList::AppendResult ValueList::append_if_value(Value &&value)
    {
        return this->append_if(value.has_nonempty_value(), std::move(value));
    }

    void ValueList::to_stream(std::ostream &stream) const
    {
        stream << "[";
        bool comma = false;
        for (const Value &element : *this)
        {
            if (comma)
            {
                stream << ", ";
            }
            else
            {
                comma = true;
            }
            element.to_literal_stream(stream);
        }
        stream << "]";
    }


}  // namespace core::types
