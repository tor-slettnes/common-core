// -*- c++ -*-
//==============================================================================
/// @file partslist.c++
/// @brief List of tag/value pairs used to construct string represenations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "partslist.h++"

namespace core::types
{
    void PartsList::add_string(const Tag &tag,
                               const std::string &value,
                               const std::optional<bool> &condition,
                               const std::string &format)
    {
        this->add_if(condition.value_or(!value.empty()), tag, value, format);
    }

    void PartsList::add_value(const Tag &tag,
                              const Value &value,
                              const std::optional<bool> &condition,
                              const std::string &format)
    {
        this->add_if(condition.value_or(!value.empty()), tag, value, format);
    }

    TaggedValueList PartsList::as_tvlist() const
    {
        TaggedValueList tvlist;
        for (const auto &[tag, value, format] : *this)
        {
            tvlist.emplace_back(tag, value);
        }
        return tvlist;
    }

    void PartsList::add_if(bool condition,
                           const Tag &tag,
                           const Value &value,
                           const Format &format)
    {
        if (condition)
        {
            this->emplace_back(tag, value, format);
        }
    }

    void PartsList::to_stream(std::ostream &stream) const
    {
        stream << "{";
        std::string delimiter = "";
        for (const auto &[tag, value, format] : *this)
        {
            stream << delimiter;
            if (tag)
            {
                stream << *tag << "=";
            }
            stream << str::format(format, value);
            delimiter = ", ";
        }
        stream << "}";
    }

}  // namespace core::types
