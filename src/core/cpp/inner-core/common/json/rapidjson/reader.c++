/// -*- c++ -*-
//==============================================================================
/// @file reader.c++
/// @brief Read JSON file
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "reader.h++"
#include "status/exceptions.h++"

#include <rapidjson/error/en.h>

#include <fstream>

namespace core::json
{
    RapidReader::RapidReader()
        : Super("RapidReader")
    {
    }

    // types::Value RapidReader::decoded_with_comments(const std::string &text) const
    // {
    //     return RapidReader::decoded(RapidReader::uncomment(text));
    // }

    types::Value RapidReader::decoded(const std::string_view &text) const
    {
        ::rapidjson::Document doc;
        doc.Parse(text.data(), text.size());
        if (doc.HasParseError())
        {
            std::size_t offset(doc.GetErrorOffset());
            std::string msg(GetParseError_En(doc.GetParseError()));
            throw exception::FailedPostcondition(msg, {{"offset", offset}});
        }
        return RapidReader::decodeValue(doc);
    }

    types::Value RapidReader::read_file(const fs::path &path) const
    {
        return this->decoded(This::read_text_from_file(path));
    }

    types::Value RapidReader::read_stream(std::istream &stream) const
    {
        if (auto *ss = dynamic_cast<std::stringstream*>(&stream))
        {
            return this->decoded(ss->str());
        }
        else
        {
            stream.seekg(0, std::ios_base::end);
            std::size_t size = stream.tellg();
            stream.seekg(0, std::ios_base::beg);
            std::string string(size, '\0');
            stream.read(string.data(), string.size());
            return this->decoded(string);
        }
    }

    types::Value RapidReader::read_stream(std::istream &&stream) const
    {
        return this->read_stream(stream);
    }


    std::string RapidReader::read_text_from_file(const fs::path &path)
    {
        std::size_t size = fs::file_size(path);

        if (std::ifstream is{path})
        {
            std::string str(size, '\0');
            is.read(&str[0], size);
            return str;
        }
        else
        {
            return {};
        }
    }

    // std::string RapidReader::uncomment(const std::string &text)
    // {
    //     static const std::regex rx(
    //         "(#.*?(?:$|\\r|\\n))|"       // (1) Script-style comments, throw away
    //         "(//.*?(?:$|\\r|\\n))|"      // (2) C++ style comments, throw away
    //         "(/\\*.*?\\*/)|"             // (3) C-style comments, throw away
    //         "('(?:\\\\.|[^\\'])*')|"     // (4) Single-quoted strings, retain
    //         "(\"(?:\\\\.|[^\\\"])*\")",  // (5) Double-quoted strings, retain
    //         std::regex::ECMAScript);

    //     auto rx_begin = std::sregex_iterator(text.begin(), text.end(), rx);
    //     auto rx_end = std::sregex_iterator();
    //     uint pos = 0, next = 0;
    //     std::vector<std::string> parts;

    //     parts.reserve(3 * std::distance(rx_begin, rx_end) + 1);

    //     for (std::regex_iterator it = rx_begin; it != rx_end; it++)
    //     {
    //         pos = (uint)it->position(0);
    //         parts.push_back(text.substr(next, pos - next));  // Keep text leading up to match
    //         parts.push_back(it->str(4));                     // Keep single-quoted strings
    //         parts.push_back(it->str(5));                     // Keep double-quoted strings
    //         next = pos + (uint)it->length(0);
    //     }
    //     parts.push_back(text.substr(next));
    //     return str::join(parts, "");
    // }

    types::Value RapidReader::decodeValue(const ::rapidjson::Value &jv)
    {
        switch (jv.GetType())
        {
        case ::rapidjson::Type::kNullType:
            return {};

        case ::rapidjson::Type::kFalseType:
            return false;

        case ::rapidjson::Type::kTrueType:
            return true;

        case ::rapidjson::Type::kObjectType:
            return This::decodeObject(jv);

        case ::rapidjson::Type::kArrayType:
            return This::decodeArray(jv);

        case ::rapidjson::Type::kStringType:
            return std::string(jv.GetString(), jv.GetStringLength());

        case ::rapidjson::Type::kNumberType:
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

    types::ValueListPtr RapidReader::decodeArray(const ::rapidjson::Value &jarray)
    {
        auto list = types::ValueList::create_shared();
        for (auto it = jarray.Begin(); it != jarray.End(); it++)
        {
            list->push_back(This::decodeValue(*it));
        }
        return list;
    }

    types::KeyValueMapPtr RapidReader::decodeObject(const ::rapidjson::Value &jobject)
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
