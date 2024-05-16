/// -*- c++ -*-
//==============================================================================
/// @file variant-types.h++
/// @brief Misc type definitions for variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/bytevector.h++"
#include "types/symbolmap.h++"
#include "chrono/date-time.h++"

#include <complex>
#include <iostream>
#include <string>
#include <map>
#include <variant>
#include <vector>
#include <deque>

namespace cc::types
{
    /// Classes implemented here.
    class Boolean;
    class Value;
    class KeyValueMap;
    class ValueList;
    class TaggedValueList;

    using NullValueType = std::monostate;
    using Key = std::string;
    using Tag = std::optional<std::string>;
    using TagList = std::vector<Tag>;

    using KeyValuePair = std::pair<std::string, Value>;
    using TaggedValue = std::pair<Tag, Value>;

    using KeyValueMapRef = std::shared_ptr<KeyValueMap>;
    using ValueListRef = std::shared_ptr<ValueList>;
    using TaggedValueListRef = std::shared_ptr<TaggedValueList>;

    /// Type aliases
    using largest_uint = std::uintmax_t;
    using largest_sint = std::intmax_t;
    using ushort = unsigned short;
    using complex = std::complex<double>;
    using string = std::string;

    using ValueBase = std::variant<
        NullValueType,
        bool,
        char,
        largest_uint,
        largest_sint,
        double,
        complex,
        string,
        ByteVector,
        dt::TimePoint,
        dt::Duration,
        ValueListRef,
        KeyValueMapRef,
        TaggedValueListRef>;

    /// Enumeration of different types corresponding to ValueBase::index().
    enum class ValueType
    {
        NONE,
        BOOL,
        CHAR,
        UINT,
        SINT,
        REAL,
        COMPLEX,
        STRING,
        BYTEVECTOR,
        TIMEPOINT,
        DURATION,
        VALUELIST,
        KVMAP,
        TVLIST,
    };

    /// Tag portion of a tag/value pair
    extern const Tag nulltag;

    /// Empty value
    extern const NullValueType nullvalue;

    /// Keys for storing complex numbers as Key/Value pairs
    extern const std::string REAL_PART;
    extern const std::string IMAG_PART;

    extern const SymbolMap<ValueType, std::multimap<ValueType, std::string>> TypeNames;

    std::ostream &operator<<(std::ostream &stream, ValueType type);
    std::istream &operator>>(std::istream &stream, ValueType &type);

    extern std::string typetree(const Value &value);
    extern std::string typetree(const ValueListRef &list);
    extern std::string typetree(const KeyValueMapRef &kvmap);
    extern std::string typetree(const TaggedValueListRef &tvlist);

}  // namespace cc::types
