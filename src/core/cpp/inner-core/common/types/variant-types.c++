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

namespace core::types
{
    const Tag nulltag;
    const NullValueType nullvalue;

    const std::string REAL_PART{"real"};
    const std::string IMAG_PART{"imag"};

    //==========================================================================
    // types::Value class methods

    const SymbolMap<ValueType, std::multimap<ValueType, std::string>> TypeNames = {
        {ValueType::NONE, "null"},
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
        {ValueType::VALUELIST, "list"},
        {ValueType::TVLIST, "TaggedValueList"},
        {ValueType::TVLIST, "tvlist"},
        {ValueType::KVMAP, "KeyValueMap"},
        {ValueType::KVMAP, "map"},
    };  // namespace types

    std::ostream &operator<<(std::ostream &stream, ValueType type)
    {
        return TypeNames.to_stream(stream, type, "(Unknown type)");
    }

    std::istream &operator>>(std::istream &stream, ValueType &type)
    {
        return TypeNames.from_stream(stream, &type, ValueType::NONE);
    }

    bool is_simple(ValueType vt)
    {
        return !is_composite(vt);
    }

    bool is_composite(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::VALUELIST:
        case ValueType::TVLIST:
        case ValueType::KVMAP:
            return true;
        default:
            return false;
        }
    }

    bool is_integral(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::UINT:
        case ValueType::SINT:
            return true;
        default:
            return false;
        }
    }

    bool is_real(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
            return true;
        default:
            return false;
        }
    }

    bool is_numeric(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::UINT:
        case ValueType::SINT:
        case ValueType::REAL:
        case ValueType::COMPLEX:
            return true;
        default:
            return false;
        }
    }

    bool is_time(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::TIMEPOINT:
        case ValueType::DURATION:
            return true;
        default:
            return false;
        }
    }

    bool is_text(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::CHAR:
        case ValueType::STRING:
            return true;
        default:
            return false;
        }
    }

    bool is_bytesequence(ValueType vt)
    {
        switch(vt)
        {
        case ValueType::STRING:
        case ValueType::BYTEVECTOR:
            return true;
        default:
            return false;
        }
    }

    std::string typetree(const Value &value)
    {
        switch (value.type())
        {
        case ValueType::VALUELIST:
            return typetree(std::get<ValueListPtr>(value));
        case ValueType::KVMAP:
            return typetree(std::get<KeyValueMapPtr>(value));
        case ValueType::TVLIST:
            return typetree(std::get<TaggedValueListPtr>(value));
        default:
            return value.type_name();
        }
    }

    std::string typetree(const ValueListPtr &list)
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

    std::string typetree(const KeyValueMapPtr &kvmap)
    {
        std::string delimiter;
        std::stringstream ss;
        ss << "KeyValueMap(";
        if (kvmap)
        {
            for (const auto &[k, v] : *kvmap)
            {
                ss << delimiter << k << ":" << typetree(v);
                delimiter = ", ";
            }
        }
        ss << ")";
        return ss.str();
    }

    std::string typetree(const TaggedValueListPtr &tvlist)
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

}  // namespace core::types
