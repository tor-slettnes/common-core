// -*- c++ -*-
//==============================================================================
/// @file variant-types.c++
/// @brief Misc type definitions for variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "variant-types.h++"
#include "variant-value.h++"
#include "variant-list.h++"
#include "variant-tvlist.h++"
#include "variant-kvmap.h++"

namespace cc::types
{
    const Tag nulltag;
    const NullValueType nullvalue;

    const std::string real_part{"real"};
    const std::string imag_part{"imag"};

    //==========================================================================
    // types::Value class methods

    const SymbolMap<ValueType, std::multimap<ValueType, std::string>> TypeNames = {
        {ValueType::NONE, "nullvalue"},
        {ValueType::BOOL, "bool"},
        {ValueType::CHAR, "char"},
        {ValueType::UINT, "uint"},
        {ValueType::SINT, "int"},
        {ValueType::REAL, "real"},
        {ValueType::COMPLEX, "complex"},
        {ValueType::STRING, "string"},
        {ValueType::BYTEVECTOR, "ByteVector"},
        {ValueType::TIMEPOINT, "TimePoint"},
        {ValueType::DURATION, "Duration"},
        {ValueType::VALUELIST, "ValueList"},
        {ValueType::KVMAP, "KeyValueMap"},
        {ValueType::TVLIST, "TaggedValueList"},
        {ValueType::KVMAP, "map"},
        {ValueType::VALUELIST, "list"},
    };  // namespace types

    std::ostream &operator<<(std::ostream &stream, ValueType type)
    {
        return TypeNames.to_stream(stream, type, "(Unknown type)");
    }

    std::istream &operator>>(std::istream &stream, ValueType &type)
    {
        return TypeNames.from_stream(stream, &type, ValueType::NONE);
    }

    std::string typetree(const Value &value)
    {
        switch (value.type())
        {
        case ValueType::VALUELIST:
            return typetree(std::get<ValueListRef>(value));
        case ValueType::KVMAP:
            return typetree(std::get<KeyValueMapRef>(value));
        case ValueType::TVLIST:
            return typetree(std::get<TaggedValueListRef>(value));
        default:
            return value.type_name();
        }
    }

    std::string typetree(const ValueListRef &list)
    {
        std::string delimiter;
        std::stringstream ss;
        ss << "ValueList(";

        if (list)
        {
            for (const Value &v : *list)
            {
                ss << delimiter << typetree(v);
                delimiter = ", ";
            }
        }
        ss << ")";
        return ss.str();
    }

    std::string typetree(const KeyValueMapRef &kvmap)
    {
        std::string delimiter;
        std::stringstream ss;
        ss << "KeyValueMap(";
        if (kvmap)
        {
            for (const auto &[k, v] : *kvmap)
            {
                ss << delimiter << std::quoted(k) << ": " << typetree(v);
                delimiter = ", ";
            }
        }
        ss << ")";
        return ss.str();
    }

    std::string typetree(const TaggedValueListRef &tvlist)
    {
        std::string delimiter;
        std::stringstream ss;
        ss << "TaggedValueList(";

        if (tvlist)
        {
            for (const auto &[t, v] : *tvlist)
            {
                ss << delimiter;
                if (t.has_value())
                {
                    ss << "[" << t.value() << "] ";
                }
                ss << typetree(v);
                delimiter = ", ";
            }
        }
        ss << ")";
        return ss.str();
    }

}  // namespace cc::types
