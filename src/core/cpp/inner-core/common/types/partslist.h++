/// -*- c++ -*-
//==============================================================================
/// @file partslist.h++
/// @brief List of tag/value pairs used to construct string represenations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "streamable.h++"
#include "value.h++"
#include "filesystem.h++"
#include "chrono/date-time.h++"
#include "string/format.h++"
#include <string>
#include <list>

namespace core::types
{
    using Format = std::string;

    //==========================================================================
    /// @class PartsList
    /// @brief Tag/Value pairs suitable for representation of strucutres

    class PartsList : public std::vector<std::tuple<Tag, Value, Format>>,
                      public Streamable
    {
    public:
        void add(const Tag &tag,
                 const std::string &value,
                 const std::optional<bool> &condition = {},
                 const std::string &format = "%r");

        void add(const Tag &tag,
                 const Value &value,
                 const std::optional<bool> &condition = {},
                 const std::string &format = "%s");

        template <class ValueType>
        void add(const Tag &tag,
                 const ValueType &value,
                 bool condition = true,
                 const std::string &format = "%s")
        {
            this->add_if(condition, tag, str::format(format, value), "");
        }

        TaggedValueList as_tvlist() const;

    protected:
        void add_if(bool condition,
                    const Tag &tag,
                    const Value &value,
                    const Format &format);

        void to_stream(std::ostream &stream) const override;
    };
}  // namespace core::types
