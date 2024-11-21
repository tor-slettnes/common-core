/// -*- c++ -*-
//==============================================================================
/// @file reader.h++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tokenparser-base.h++"
#include "parsers/common/basereader.h++"

namespace core::json
{
    class CustomReader : public parsers::BaseReader
    {
        using This = CustomReader;
        using Super = parsers::BaseReader;

    public:
        CustomReader();

        types::Value decoded(const std::string_view &string) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        types::Value read_stream(std::istream &&stream) const override;

    private:
        static types::Value parse_input(const TokenParser::ptr &parser);
        static types::Value parse_value(const TokenParser::ptr &parser);
        static types::KeyValueMapPtr parse_object(const TokenParser::ptr &parser);
        static types::ValueListPtr parse_array(const TokenParser::ptr &parser);

        static TokenPair next_value(const TokenParser::ptr &parser,
                                    const TokenMask &endtokens = TI_NONE);
    };
}  // namespace core::json
