/// -*- c++ -*-
//==============================================================================
/// @file jsonwriter.c++
/// @brief Write values to JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "writer.h++"
#include <fstream>

namespace cc::json
{
    CustomWriter::CustomWriter()
        : Super("CustomWriter")
    {
    }

    CustomWriter::CustomWriter(const fs::path &path)
        : Super("CustomWriter", path)
    {
    }

    void CustomWriter::write_stream(std::ostream &stream,
                                    const types::Value &value,
                                    bool pretty) const
    {
        This::to_stream(stream, value, pretty);
    }

    std::string CustomWriter::encoded(const types::Value &value,
                                      bool pretty) const
    {
        std::stringstream ss;
        This::to_stream(ss, value, pretty);
        return ss.str();
    }

    std::ostream &CustomWriter::to_stream(std::ostream &stream,
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

    std::ostream &CustomWriter::to_stream(std::ostream &stream,
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

    std::ostream &CustomWriter::to_stream(std::ostream &stream,
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

    std::ostream &CustomWriter::to_stream(
        std::ostream &stream,
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

}  // namespace cc::json
