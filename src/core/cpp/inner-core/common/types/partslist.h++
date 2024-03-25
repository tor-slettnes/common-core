/// -*- c++ -*-
//==============================================================================
/// @file partslist.h++
/// @brief List of tag/value pairs used to construct string represenations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "streamable.h++"
#include "string/format.h++"
#include <string>
#include <list>

namespace core::types
{
    //==========================================================================
    /// @class PartsList
    /// @brief Tag/Value pairs suitable for representation of strucutres

    class PartsList : public std::vector<std::string>,
                      public Streamable
    {
    public:
        template <class ValueType>
        void add(const std::string &tag,
                 const ValueType &value,
                 bool condition = true,
                 const std::string &format = "%r")
        {
            if (condition)
            {
                this->push_back(str::format("%s=" + format, tag, value));
            }
        }

        void add(const std::string &tag,
                 const std::string &value,
                 const std::string &format = "%r");

    protected:
        void to_stream(std::ostream &stream) const override;
    };
}  // namespace core::types
