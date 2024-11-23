/// -*- c++ -*-
//==============================================================================
/// @file tokenparser-json.c++
/// @brief Iterate over interesting JSON tokens
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-json.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::json
{
    const int eof = std::char_traits<char>::eof();

    JsonParser::JsonParser(parsers::Input::ptr input)
        : Super(input, JsonParser::symbol_map)
    {
    }

    parsers::TokenParser::TokenPair JsonParser::next_of(const TokenMask &expected,
                                                        const TokenMask &endtokens)
    {
        TokenPair tp = this->next_token();

        if (expected & tp.first)
        {
            return tp;
        }
        else if (endtokens & tp.first)
        {
            return TokenPair(TI_NONE, {});
        }
        else if (tp.first == TI_END)
        {
            throwf(exception::MissingArgument,
                   "Missing token at end of input");
        }
        else if (tp.first == TI_INVALID)
        {
            throwf(exception::InvalidArgument,
                   "Invalid input at position %d: %s",
                   this->input->token_position(),
                   this->input->token());
        }
        else
        {
            throwf(exception::InvalidArgument,
                   "Unexpected token at position %d: %s",
                   this->input->token_position(),
                   this->input->token());
        }
    }

    parsers::TokenParser::TokenPair JsonParser::next_token()
    {
        while (int c = this->input->getc())
        {
            this->input->init_token(c);
            switch (TokenIndex ti = this->token_index(c))
            {
            case TI_SPACE:
            case TI_ENDLINE:
                this->parse_spaces();
                continue;

            case TI_LINE_COMMENT:
                this->parse_line_comment();
                continue;

            case TI_QUOTED_STRING:
                return this->parse_string(c);

            case TI_NUMERIC:
                return this->parse_number();

            case TI_SYMBOL:
                return this->parse_symbol();

            default:
                return {ti, this->input->token()};
            }
        }

        return {TI_NONE, {}};
    }

    parsers::TokenParser::TokenIndex JsonParser::token_index(int c) const
    {
        switch (c)
        {
        case ' ':
        case '\t':
            return TI_SPACE;

        case '\r':
        case '\n':
            return TI_ENDLINE;

        case '#':
        case '/':
            return TI_LINE_COMMENT;

        case '"':
        case '\'':
            return TI_QUOTED_STRING;

        case '-':
        case '0' ... '9':
            return TI_NUMERIC;

        case 'a' ... 'z':
        case 'A' ... 'Z':
            return TI_SYMBOL;

        case '{':
            return TI_MAP_OPEN;

        case '}':
            return TI_MAP_CLOSE;

        case '[':
            return TI_LIST_OPEN;

        case ']':
            return TI_LIST_CLOSE;

        case ',':
            return TI_COMMA;

        case ':':
            return TI_COLON;

        case std::char_traits<char>::eof():
            return TI_END;

        default:
            return TI_NONE;
        }
    }

    parsers::TokenParser::TokenPair JsonParser::parse_line_comment()
    {
        TokenPair tp = Super::parse_line_comment();

        if ((this->input->token().compare(0, 2, "//") == 0) ||
            (this->input->token().compare(0, 1, "#") == 0))
        {
            return tp;
        }
        else
        {
            return {TI_INVALID, {}};
        }
    }

    const parsers::TokenParser::SymbolMapping JsonParser::symbol_map = {
        {"null", {TI_NULLVALUE, types::Value()}},
        {"false", {TI_BOOL, types::Value(false)}},
        {"true", {TI_BOOL, types::Value(true)}},
    };

}  // namespace core::json
