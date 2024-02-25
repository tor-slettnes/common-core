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

#include <fstream>

namespace shared::json
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
        return JsonDecoder::parse_text_with_comments(This::read_text(path));
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
}  // namespace shared::json
