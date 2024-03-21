/// -*- c++ -*-
//==============================================================================
/// @file jsonreader.h++
/// @brief Read JSON file, possibly with comments
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "tokenparser-base.h++"
#include "json/basereader.h++"

namespace core::json
{
    class CustomReader : public BaseReader
    {
        using This = CustomReader;
        using Super = BaseReader;

        using ParserRef = std::shared_ptr<TokenParser>;

    public:
        types::Value decoded(const std::string &string) const override;
        types::Value read_file(const fs::path &path) const override;
        types::Value read_stream(std::istream &stream) const override;
        types::Value read_stream(std::istream &&stream) const override;

    private:
        static types::Value parse_input(const ParserRef &parser);
        static types::Value parse_value(const ParserRef &parser);
        static types::KeyValueMapRef parse_object(const ParserRef &parser);
        static types::ValueListRef parse_array(const ParserRef &parser);

        static TokenPair next_value(const ParserRef &parser,
                                    const TokenMask &endtokens = TI_NONE);
    };
}  // namespace core::json
