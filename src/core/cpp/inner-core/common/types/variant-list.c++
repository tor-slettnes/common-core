// -*- c++ -*-
//==============================================================================
/// @file variant-list.c++
/// @brief Vector of `Value` instances
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "variant-list.h++"

namespace core::types
{
    void ValueList::to_stream(std::ostream &stream) const
    {
        stream << "{";
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
        stream << "}";
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

    TaggedValueList ValueList::as_tvlist() const noexcept
    {
        TaggedValueList tvlist;
        tvlist.extend(*this);
        return tvlist;
    }

}  // namespace core::types
