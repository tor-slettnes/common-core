/// -*- c++ -*-
//==============================================================================
/// @file yaml-reader.c++
/// @brief Yaml Parser Interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "yaml-reader.h++"
#include "yaml-parser.h++"

namespace core::yaml
{
    YamlReader::YamlReader()
        : Super("YamlReader")
    {
    }

    types::Value YamlReader::decoded(const std::string_view &text) const
    {
        return YamlParser().parse_text(text);
    }

    types::Value YamlReader::read_file(const fs::path &path) const
    {
        return YamlParser().parse_file(path);
    }

    types::Value YamlReader::read_stream(std::istream &stream) const
    {
        return YamlParser().parse_stream(stream);
    }
}  // namespace core::yaml
