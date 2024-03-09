/// -*- c++ -*-
//==============================================================================
/// @file jsonparser.h++
/// @brief Decode JSON stream as variant value object
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/value.h++"
#include "tokenparser-base.h++"

#include <unordered_set>
#include <memory>
#include <optional>

namespace core::json
{
    class JsonParser
    {
        using This = JsonParser;
        using ParserRef = std::shared_ptr<TokenParser>;

    public:
        static types::Value parse_text(const std::string &text);
        static types::Value parse_stream(std::istream &&istream);
        static types::Value parse_stream(std::istream &istream);

    private:
        static types::Value parse_value(const ParserRef &parser);
        static types::KeyValueMapRef parse_object(const ParserRef &parser);
        static types::ValueListRef parse_array(const ParserRef &parser);

        static TokenPair next_value(const ParserRef &parser,
                                    const TokenMask &endtokens = TI_NONE);

    };
}  // namespace core::json
