/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.c++
/// @brief Iterate over interesting JSON tokens
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::json
{
    const int eof = std::char_traits<char>::eof();

    TokenParser::TokenParser(parsers::Input::ptr input)
        : input(input)
    {
    }


    TokenParser::TokenPair TokenParser::next_of(const TokenMask &expected,
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

    TokenParser::TokenPair TokenParser::next_token()
    {
        while (int c = this->input->getc())
        {
            this->input->init_token(c);
            switch (TokenIndex ti = this->token_index(c))
            {
            case TI_SPACE:
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

    TokenParser::TokenIndex TokenParser::token_index(int c) const
    {
        switch (c)
        {
        case ' ':
        case '\t':
        case '\v':

        case '\r':
        case '\n':
            return TI_SPACE;

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


    TokenParser::TokenPair TokenParser::parse_spaces()
    {
        int c = this->input->getc();
        while (std::isspace(c))
        {
            this->input->append_to_token(c);
            c = this->input->getc();
        }
        this->input->ungetc(c);
        return {TI_SPACE, this->input->token()};
    }



    TokenParser::TokenPair TokenParser::parse_number()
    {
        int c = '\0';
        bool got_sign = (*this->input->token().begin() == '-');
        bool got_real = (*this->input->token().begin() == '.');
        bool got_hex = false;

        for (bool is_numeric = true; is_numeric;)
        {
            c = this->input->getc();
            switch (c)
            {
            case '0' ... '9':
                this->input->append_to_token(c);
                break;

            case '.':
            case 'e':
            case 'E':
                got_real = true;
                this->input->append_to_token(c);
                break;

            case 'x':
            case 'X':
                got_hex = true;
                this->input->append_to_token(c);
                break;

            default:
                is_numeric = false;
                break;
            }
        }
        this->input->ungetc(c);

        types::Value value;
        if (got_real)
        {
            return this->parse_numeric<double>();
        }
        else if (got_sign)
        {
            return this->parse_numeric<std::int64_t>();
        }
        else if (got_hex)
        {
            return this->parse_numeric<std::uint64_t>(0);
        }
        else
        {
            return this->parse_numeric<std::uint64_t>();
        }
    }

    TokenParser::TokenPair TokenParser::parse_symbol()
    {
        this->capture_identifier();
        try
        {
            return this->symbol_map.at(this->input->token());
        }
        catch (const std::out_of_range &e)
        {
            return {TI_INVALID, {}};
        }
    }

    TokenParser::TokenPair TokenParser::parse_string(char quote, bool raw)
    {
        int c = '\0';
        bool escape = false;
        std::string string;
        std::size_t size = 0;
        std::size_t capacity = 64;
        string.reserve(capacity);

        while ((c = this->input->getc()) != eof)
        {
            this->input->append_to_token(c);

            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\' && !raw)
            {
                escape = true;
                continue;
            }
            else if (c == quote)
            {
                return {TI_QUOTED_STRING, string};
            }

            if (++size >= capacity)
            {
                capacity = std::max(std::size_t(64), 2 * capacity);
                string.reserve(capacity);
            }
            string.push_back(c);
        }

        return {TI_END, string};
    }

    TokenParser::TokenPair TokenParser::parse_line_comment()
    {
        int c = '\0';
        while ((c = this->input->getc()) != eof)
        {
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
            this->input->append_to_token(c);
        }

        if ((this->input->token().compare(0, 2, "//") == 0) ||
            (this->input->token().compare(0, 1, "#") == 0))
        {
            return {TI_LINE_COMMENT, {}};
        }
        else
        {
            return {TI_INVALID, {}};
        }
    }

    template <class T, class... Args>
    TokenParser::TokenPair TokenParser::parse_numeric(Args &&...args)
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


    void TokenParser::capture_identifier()
    {
        int c = '\0';
        for (bool is_symbolic = true; is_symbolic;)
        {
            c = this->input->getc();
            switch (c)
            {
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '0' ... '9':
            case '-':
            case '_':
                this->input->append_to_token(c);
                break;

            default:
                is_symbolic = false;
                break;
            }
        }
        this->input->ungetc(c);
    }

    char TokenParser::escape(char c)
    {
        switch (c)
        {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 'f':
            return '\f';
        case 'e':
            return '\x1b';
        default:
            return c;
        }
    }




    const TokenParser::SymbolMapping TokenParser::symbol_map = {
        {"null",  {TI_NULLVALUE, types::Value()}},
        {"false", {TI_BOOL, types::Value(false)}},
        {"true",  {TI_BOOL, types::Value(true)} },
    };

} // namespace core::json
