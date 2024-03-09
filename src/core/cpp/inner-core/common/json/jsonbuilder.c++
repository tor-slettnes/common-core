/// -*- c++ -*-
//==============================================================================
/// @file jsonbuilder.c++
/// @brief Convert a Value instance to JSON
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonbuilder.h++"

namespace core::json
{
    std::string JsonBuilder::to_string(const types::Value &value,
                                       bool pretty)
    {
        std::stringstream ss;
        This::to_stream(ss, value, pretty);
        return ss.str();
    }

    std::ostream &JsonBuilder::to_stream(std::ostream &&stream,
                                         const types::Value &value,
                                         bool pretty,
                                         const std::string &indent)
    {
        return This::to_stream(stream, value, pretty, indent);
    }

    std::ostream &JsonBuilder::to_stream(std::ostream &stream,
                                         const types::Value &value,
                                         bool pretty,
                                         const std::string &indent)
    {
        switch (value.type())
        {
        case types::ValueType::BYTEVECTOR:
            This::to_stream(stream,
                            types::ValueList(value.get<types::ByteVector>().begin(),
                                             value.get<types::ByteVector>().end()),
                            pretty,
                            indent);
            break;

        case types::ValueType::KVMAP:
            This::to_stream(stream, *value.get_kvmap(), pretty, indent);
            break;

        case types::ValueType::VALUELIST:
            This::to_stream(stream, *value.get_valuelist(), pretty, indent);
            break;

        case types::ValueType::TVLIST:
            This::to_stream(stream, *value.get_tvlist(), pretty, indent);
            break;

        default:
            value.to_literal_stream(stream);
            break;
        }
        return stream;
    }

    std::ostream &JsonBuilder::to_stream(std::ostream &stream,
                                         const types::KeyValueMap &kvmap,
                                         bool pretty,
                                         const std::string &indent)
    {
        std::string sub_indent;
        std::string delimiter;
        std::string kvseparator = ":";
        std::string infix;

        if (pretty)
        {
            infix = "\n" + indent;
            sub_indent = "  ";
            kvseparator = ": ";
        }

        stream << "{";
        for (const auto &[key, value] : kvmap)
        {
            stream << delimiter
                   << infix
                   << sub_indent
                   << std::quoted(key)
                   << kvseparator;

            This::to_stream(stream, value, pretty, indent + sub_indent);
            delimiter = ",";
        }
        stream << infix << "}";
        return stream;
    }

    std::ostream &JsonBuilder::to_stream(std::ostream &stream,
                                         const types::ValueList &list,
                                         bool pretty,
                                         const std::string &indent)
    {
        std::string sub_indent;
        std::string delimiter;
        std::string infix;

        if (pretty)
        {
            infix = "\n" + indent;
            sub_indent = "  ";
        }

        stream << "[";
        for (const types::Value &value : list)
        {
            stream << delimiter
                   << infix
                   << sub_indent;

            This::to_stream(stream, value, pretty, indent + sub_indent);
            delimiter = ",";
        }
        stream << infix << "]";
        return stream;
    }

    std::ostream &JsonBuilder::to_stream(std::ostream &stream,
                                         const types::TaggedValueList &tvlist,
                                         bool pretty,
                                         const std::string &indent)

    {
        if (tvlist.mappable())
        {
            return This::to_stream(stream, tvlist.as_kvmap(), pretty, indent);
        }
        else if (!tvlist.tagged())
        {
            return This::to_stream(stream, tvlist.values(), pretty, indent);
        }
        else
        {
            std::string sub_indent;
            std::string delimiter;
            std::string tvseparator = ",";
            std::string infix;

            if (pretty)
            {
                infix = "\n" + indent;
                sub_indent = "  ";
                tvseparator = ", ";
            }

            stream << "[";
            for (const auto &[tag, value] : tvlist)
            {
                stream << delimiter
                       << infix
                       << sub_indent;

                This::to_stream(stream, types::Value(tag), pretty, indent);
                stream << tvseparator;
                This::to_stream(stream, value, pretty, indent + sub_indent);
                delimiter = ",";
            }
            stream << infix << "]";
            return stream;
        }
    }

}  // namespace core::json
