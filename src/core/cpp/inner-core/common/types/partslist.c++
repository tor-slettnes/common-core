// -*- c++ -*-
//==============================================================================
/// @file partslist.c++
/// @brief List of tag/value pairs used to construct string represenations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "partslist.h++"

namespace core::types
{
    void PartsList::add(const std::string &tag,
                        const std::string &value,
                        const std::string &format)
    {
        if (value.size())
        {
            this->push_back(str::format("%s=" + format, tag, value));
        }
    }

    void PartsList::to_stream(std::ostream &stream) const
    {
        stream << "{";
        std::string delimiter = "";
        for (const auto &item : *this)
        {
            stream << delimiter << item;
            delimiter = ", ";
        }
        stream << "}";
    }

}  // namespace core::types
