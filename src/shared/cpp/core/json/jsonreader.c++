/// -*- c++ -*-
//==============================================================================
/// @file jsonreader.c++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "jsonreader.h++"
#include "jsondecoder.h++"
#include "status/exceptions.h++"

#include <rapidjson/filereadstream.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>

#include <fstream>

namespace cc::json
{
    JsonReader::JsonReader(const fs::path &path)
        : path_(path)
    {
    }

    types::Value JsonReader::read()
    {
        return This::read_from(this->path_);
    }

    types::Value JsonReader::read_from(const fs::path &path)
    {
        return This::parse_text(This::uncomment(This::read_text(path)));
    }

    std::string JsonReader::read_text(const fs::path &path)
    {
        std::uintmax_t size = fs::file_size(path);

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

    std::string JsonReader::uncomment(const std::string &text)
    {
        std::regex rx(
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

    types::Value JsonReader::parse_text(std::string &&text)
    {
        rapidjson::Document doc;
        doc.ParseInsitu(text.data());
        if (doc.HasParseError())
        {
            std::size_t offset(doc.GetErrorOffset());
            std::string msg(GetParseError_En(doc.GetParseError()));
            throw exception::FailedPostcondition(msg, {{"offset", offset}});
        }
        return JsonDecoder::decodeValue(doc);
    }
}  // namespace cc::json
