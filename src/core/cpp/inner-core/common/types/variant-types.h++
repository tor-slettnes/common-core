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

namespace core::types
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

    using KeyValueMapPtr = std::shared_ptr<KeyValueMap>;
    using ValueListPtr = std::shared_ptr<ValueList>;
    using TaggedValueListPtr = std::shared_ptr<TaggedValueList>;

    /// Type aliases
    using largest_uint = std::uintmax_t;
    using largest_sint = std::intmax_t;
    using largest_real = double;
    using ushort = unsigned short;
    using complex = std::complex<largest_real>;
    using string = std::string;

    using ValueBase = std::variant<
        NullValueType,
        bool,
        char,
        largest_uint,
        largest_sint,
        largest_real,
        complex,
        string,
        ByteVector,
        dt::TimePoint,
        dt::Duration,
        ValueListPtr,
        TaggedValueListPtr,
        KeyValueMapPtr>;

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
        TVLIST,
        KVMAP,
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

    extern bool is_simple(ValueType vt);
    extern bool is_composite(ValueType vt);
    extern bool is_integral(ValueType vt);
    extern bool is_real(ValueType vt);
    extern bool is_numeric(ValueType vt);
    extern bool is_time(ValueType vt);
    extern bool is_text(ValueType vt);
    extern bool is_bytesequence(ValueType vt);

    extern std::string typetree(const Value &value);
    extern std::string typetree(const ValueListPtr &list);
    extern std::string typetree(const KeyValueMapPtr &kvmap);
    extern std::string typetree(const TaggedValueListPtr &tvlist);

}  // namespace core::types
