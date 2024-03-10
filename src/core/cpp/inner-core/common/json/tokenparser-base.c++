/// -*- c++ -*-
//==============================================================================
/// @file tokenparser.c++
/// @brief Iterate over interesting tokens in a string
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "tokenparser-base.h++"
#include "status/exceptions.h++"
#include "logging/logging.h++"

#include <regex>
#include <sstream>
#include <charconv>
#include <string>

namespace core::json
{
    const int eof = std::char_traits<char>::eof();

    TokenPair TokenParser::next_of(const TokenMask &expected,
                                   const TokenMask &endtokens)
    {
        TokenPair tp = this->next_token();
        while (tp.first == TI_LINE_COMMENT)
        {
            tp = this->next_token();
        }

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
                   "Missing JSON token at end of input");
        }
        else if (tp.first == TI_INVALID)
        {
            throwf(exception::InvalidArgument,
                   "Invalid input at position %d: %s",
                   this->token_position(),
                   this->token());
        }
        else
        {
            throwf(exception::InvalidArgument,
                   "Unexpected token at position %d: %s",
                   this->token_position(),
                   this->token());
        }
    }

    TokenPair TokenParser::next_token()
    {
        int c = this->getc();

        while (TokenIndex ti = this->token_index(c))
        {
            if (ti == TI_SPACE)
            {
                c = this->getc();
            }
            else
            {
                this->init_token(c);
                return {ti, {}};
            }
        }

        this->init_token(c);
        switch (c)
        {
        case '#':
        case '/':
            return this->parse_line_comment();

        case '"':
            return this->parse_string();

        case '-':
        case '0' ... '9':
            return this->parse_number();

        case 'a' ... 'z':
        case 'A' ... 'Z':
            return this->parse_symbol();

        default:
            return {TI_INVALID, {}};
        }
    }

    TokenIndex TokenParser::token_index(int c) const
    {
        switch (c)
        {
        case std::char_traits<char>::eof():
            return TI_END;

        case ' ':
        case '\r':
        case '\n':
        case '\t':
        case '\v':
        case '\f':
            return TI_SPACE;

        case '{':
            return TI_OBJECT_OPEN;

        case '}':
            return TI_OBJECT_CLOSE;

        case '[':
            return TI_ARRAY_OPEN;

        case ']':
            return TI_ARRAY_CLOSE;

        case ',':
            return TI_COMMA;

        case ':':
            return TI_COLON;

        default:
            return TI_NONE;
        }
    }

    template <class T, class... Args>
    TokenPair TokenParser::parse_numeric(Args &&...args)
    {
        static const std::errc ok{};
        const char *const start = &*this->token_begin();
        const char *const end = &*this->token_end();
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

    TokenPair TokenParser::parse_number()
    {
        int c;
        bool got_sign = (*this->token_begin() == '-');
        bool got_real = (*this->token_begin() == '.');
        bool got_hex = false;

        while (this->token_index(c = this->getc()) == TI_NONE)
        {
            this->append_to_token(c);
            switch (c)
            {
            case '.':
            case 'e':
            case 'E':
                got_real = true;
                break;

            case 'x':
            case 'X':
                got_hex = true;
                break;
            }
        }
        this->ungetc(c);

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

    TokenPair TokenParser::parse_symbol()
    {
        static const std::unordered_map<
            std::string,
            std::pair<TokenIndex, types::Value>>
            symbol_map = {
                {"null", {TI_NULL, types::Value()}},
                {"false", {TI_BOOL, types::Value(false)}},
                {"true", {TI_BOOL, types::Value(true)}},
            };

        int c;
        while (this->token_index(c = this->getc()) == TI_NONE)
        {
            this->append_to_token(c);
        }
        this->ungetc(c);

        try
        {
            return symbol_map.at(std::string(this->token_begin(), this->token_end()));
        }
        catch (const std::out_of_range &e)
        {
            return {TI_INVALID, {}};
        }
    }

    TokenPair TokenParser::parse_line_comment()
    {
        int c = '\0';
        while ((c = this->getc()) != eof)
        {
            if (std::isspace(c) && !std::isblank(c))
            {
                break;
            }
            this->append_to_token(c);
        }
        if (str::startswith(this->token(), "//") ||
            str::startswith(this->token(), "#"))
        {
            return {TI_LINE_COMMENT, {}};
        }
        else
        {
            return {TI_INVALID, {}};
        }
    }

    TokenPair TokenParser::parse_string()
    {
        int c = '\0';
        bool escape = false;
        std::string string;
        std::size_t size = 0;
        std::size_t capacity = 64;
        string.reserve(capacity);

        while ((c = this->getc()) != eof)
        {
            this->append_to_token(c);

            if (escape)
            {
                c = this->escape(c);
                escape = false;
            }
            else if (c == '\\')
            {
                escape = true;
                continue;
            }
            else if (c == '"')
            {
                return {TI_STRING, string};
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

}  // namespace core::json
