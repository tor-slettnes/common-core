/// -*- c++ -*-
//==============================================================================
/// @file jsondecoder.c++
/// @brief Decode rapidjson as variant values
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsondecoder.h++"
#include "status/exceptions.h++"
#include <rapidjson/error/en.h>

namespace core::json
{
    types::Value JsonDecoder::parse_text_with_comments(const std::string &text)
    {
        return JsonDecoder::parse_text(JsonDecoder::uncomment(text));
    }

    types::Value JsonDecoder::parse_text(std::string &&text)
    {
        rapidjson::Document doc;
        doc.Parse(text.data(), text.size());
        if (doc.HasParseError())
        {
            std::size_t offset(doc.GetErrorOffset());
            std::string msg(GetParseError_En(doc.GetParseError()));
            throw exception::FailedPostcondition(msg, {{"offset", offset}});
        }
        return JsonDecoder::decodeValue(doc);
    }

    // types::Value JsonDecoder::parse_stream(std::istream &&stream)
    // {
    //     return JsonDecoder::parse_stream(stream);
    // }

    // types::Value JsonDecoder::parse_stream(std::istream &stream)
    // {
    //     rapidjson::Document doc;
    //     doc.ParseStream(stream);
    //     if (doc.HasParseError())
    //     {
    //         std::size_t offset(doc.GetErrorOffset());
    //         std::string msg(GetParseError_En(doc.GetParseError()));
    //         throw exception::FailedPostcondition(msg, {{"offset", offset}});
    //     }
    //     return JsonDecoder::decodeValue(doc);
    // }

    std::string JsonDecoder::uncomment(const std::string &text)
    {
        static std::regex rx(
            "(#.*?(?:$|\\r|\\n))|"       // (1) Script-style comments, throw away
            "(//.*?(?:$|\\r|\\n))|"      // (2) C++ style comments, throw away
            "(/\\*.*?\\*/)|"             // (3) C-style comments, throw away
            "('(?:\\\\.|[^\\'])*')|"     // (4) Single-quoted strings, retain
            "(\"(?:\\\\.|[^\\\"])*\")",  // (5) Double-quoted strings, retain
            std::regex::ECMAScript);

        auto rx_begin = std::sregex_iterator(text.begin(), text.end(), rx);
        auto rx_end = std::sregex_iterator();
        uint pos = 0, next = 0;
        std::vector<std::string> parts;

        parts.reserve(3 * std::distance(rx_begin, rx_end) + 1);

        for (std::regex_iterator it = rx_begin; it != rx_end; it++)
        {
            pos = (uint)it->position(0);
            parts.push_back(text.substr(next, pos - next));  // Keep text leading up to match
            parts.push_back(it->str(4));                     // Keep single-quoted strings
            parts.push_back(it->str(5));                     // Keep double-quoted strings
            next = pos + (uint)it->length(0);
        }
        parts.push_back(text.substr(next));
        return str::join(parts, "");
    }

    types::Value JsonDecoder::decodeValue(const rapidjson::Value &jv)
    {
        switch (jv.GetType())
        {
        case rapidjson::Type::kNullType:
            return {};

        case rapidjson::Type::kFalseType:
            return false;

        case rapidjson::Type::kTrueType:
            return true;

        case rapidjson::Type::kObjectType:
            return This::decodeObject(jv);

        case rapidjson::Type::kArrayType:
            return This::decodeArray(jv);

        case rapidjson::Type::kStringType:
            return std::string(jv.GetString(), jv.GetStringLength());

        case rapidjson::Type::kNumberType:
            if (jv.IsUint64())
            {
                return jv.GetUint64();
            }
            else if (jv.IsInt64())
            {
                return jv.GetInt64();
            }
            else if (jv.IsUint())
            {
                return jv.GetUint();
            }
            else if (jv.IsInt())
            {
                return jv.GetInt();
            }
            else
            {
                return jv.GetDouble();
            }
            break;

        default:
            return {};
        }
    }

    types::ValueListRef JsonDecoder::decodeArray(const rapidjson::Value &jarray)
    {
        auto list = types::ValueList::create_shared();
        for (auto it = jarray.Begin(); it != jarray.End(); it++)
        {
            list->push_back(This::decodeValue(*it));
        }
        return list;
    }

    types::KeyValueMapRef JsonDecoder::decodeObject(const rapidjson::Value &jobject)
    {
        auto kvmap = types::KeyValueMap::create_shared();
        for (auto &&item : jobject.GetObject())
        {
            kvmap->insert_or_assign(item.name.GetString(),
                                    This::decodeValue(item.value));
        }
        return kvmap;
    }
}  // namespace core::json
