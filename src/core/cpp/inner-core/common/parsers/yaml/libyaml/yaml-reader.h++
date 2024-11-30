/// -*- c++ -*-
//==============================================================================
/// @file yaml-reader.h++
/// @brief Yaml Parser Interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parsers/common/basereader.h++"

namespace core::yaml
{
    class YamlReader : public parsers::BaseReader
    {
        using This = YamlReader;
        using Super = parsers::BaseReader;

    public:
        YamlReader();

        types::Value decoded(const std::string_view &text) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        using Super::read_stream;
    };
}  // namespace core::yaml
