/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-json.h++
/// @brief Iterate over interesting JSON tokens
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parsers/common/tokenparser-base.h++"

namespace core::json
{
    class JsonParser : public parsers::TokenParser
    {
        using This = JsonParser;
        using Super = parsers::TokenParser;

        using SymbolMapping =  std::unordered_map<std::string_view, TokenPair>;

    public:
        JsonParser(parsers::Input::ptr input);

        TokenPair next_of(const TokenMask &expected,
                          const TokenMask &endtokens = TI_NONE);

    protected:
        TokenPair next_token();
        TokenIndex token_index(int c) const;
        TokenPair parse_line_comment() override;

    private:
        static const SymbolMapping symbol_map;
    };
}  // namespace core::json
