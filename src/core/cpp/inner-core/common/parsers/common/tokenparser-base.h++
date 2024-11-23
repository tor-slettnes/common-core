/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-base.h++
/// @brief Iterate over interesting tokens in provided text input
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "parserinput-base.h++"
#include "types/value.h++"

#include <charconv>

namespace core::parsers
{
    class TokenParser
    {
    public:
        enum TokenIndex
        {
            TI_NONE = 0x0000,
            TI_INVALID = 0x0001,
            TI_SPACE = 0x0002,
            TI_ENDLINE = 0x0004,
            TI_MAP_OPEN = 0x0008,
            TI_MAP_CLOSE = 0x0010,
            TI_LIST_OPEN = 0x0020,
            TI_LIST_CLOSE = 0x0040,
            TI_COMMA = 0x0080,
            TI_COLON = 0x0100,
            TI_NULLVALUE = 0x0200,
            TI_BOOL = 0x0400,
            TI_NUMERIC = 0x0800,
            TI_SYMBOL = 0x1000,
            TI_QUOTED_STRING = 0x2000,
            TI_LINE_COMMENT = 0x40000000,
            TI_END = 0x80000000
        };

        using TokenMask = std::uint64_t;
        using TokenPair = std::pair<TokenIndex, types::Value>;
        using CommentStyles = std::vector<std::string_view>;
        using SymbolMapping = std::unordered_map<std::string_view, TokenPair>;

        using ptr = std::shared_ptr<TokenParser>;

    public:
        TokenParser(parsers::Input::ptr input,
                    const SymbolMapping &symbol_map = {});

    protected:
        virtual TokenPair parse_spaces();
        virtual TokenPair parse_number();
        virtual TokenPair parse_symbol();
        virtual TokenPair parse_line_comment();
        virtual TokenPair parse_string(char quote, bool raw = false);

        void capture_identifier();
        char escape(char c);

    protected:
        template <class T, class... Args>
        TokenPair parse_numeric(Args &&...args)
        {
            static const std::errc ok{};
            const char *const start = &*this->input->token().begin();
            const char *const end = &*this->input->token().end();
            T number = 0;

            auto [ptr, ec] = std::from_chars(start, end, number, args...);
            if (ec == ok)
            {
                return {TI_NUMERIC, number};
            }
            else
            {
                return {TI_INVALID, {}};
            }
        }

    protected:
        parsers::Input::ptr input;

    private:
        const SymbolMapping &symbol_map;
        std::list<std::string> indentation_stack;
    };
}  // namespace core::parsers
