/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Parse YAML content
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "tokenparser-yaml.h++"
#include "parsers/common/basereader.h++"

namespace core::yaml
{
    class Reader : public parsers::BaseReader
    {
        using This = Reader;
        using Super = parsers::BaseReader;

    public:
        Reader();

        types::Value decoded(const std::string_view &string) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &&stream) const override;
        types::Value read_stream(std::istream &stream) const override;

    private:
        uint indentation;
        std::list<uint> indentation_stack;
    };
}  // namespace core::yaml
