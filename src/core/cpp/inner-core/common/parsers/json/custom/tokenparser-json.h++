/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.h++
/// @brief Iterate over interesting tokens in a string
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

    public:
        JsonParser(parsers::Input::ptr input);
        TokenPair next_token() override;
        TokenIndex token_index(int c) const override;

    private:
        static SymbolMapping symbol_map;
    };
}  // namespace core::json
