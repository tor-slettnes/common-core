/// -*- c++ -*-
//==============================================================================
/// @file misc.c++
/// @brief Miscellaneous string manipulations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/misc.h++"

#include <algorithm>
#include <locale>
#include <string>
#include <codecvt>
#include <cwchar>
#include <utility>  // std::move()

std::string operator""_u(const char *str, std::size_t len)
{
    return core::str::to_unicode_text(std::string(str, len));
}

namespace core::str
{

    void toupper(std::string *s, const std::locale &loc)
    {
        for (auto &c : *s)
        {
            c = std::toupper(c, loc);
        }
    }

    void tolower(std::string *s, const std::locale &loc)
    {
        for (auto &c : *s)
        {
            c = std::tolower(c, loc);
        }
    }

    std::string toupper(std::string s, const std::locale &loc)
    {
        toupper(&s, loc);
        return s;
    }

    std::string tolower(std::string s, const std::locale &loc)
    {
        tolower(&s, loc);
        return s;
    }

    std::string obfuscated(const std::string_view &s)
    {
        static const std::string bullet = "\u2022";
        std::stringstream ss;
        for (auto it = s.begin(); it != s.end(); it++)
        {
            ss << bullet;
        }
        return ss.str();
    }

    std::string from_wstring(const std::wstring &wstr, const std::locale &loc)
    {
        return from_wstring(wstr.data(), wstr.size(), loc);
    }

    std::string from_wstring(const wchar_t *from, std::size_t size, const std::locale &loc)
    {
        auto &f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);

        std::mbstate_t mb;
        std::string to(size * f.max_length(), '\0');
        const wchar_t *from_next = from;
        char *to_next = to.data();

        std::codecvt_base::result result = f.out(
            mb,                     // state
            from,                   // from
            from + size,            // from_end
            from_next,              // from_next
            to.data(),              // to
            to.data() + to.size(),  // to_end
            to_next);               // to_next

        if (result == std::codecvt_base::result::error)
        {
            throw std::invalid_argument(
                "Invalid " +
                loc.name() +
                " wide character in input string at position " +
                std::to_string(from_next - from));
        }

        to.resize(to_next - to.data());
        return to;

        // static std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
        // return conv.to_bytes(wstr);
    }

    std::wstring to_wstring(const std::string &str, const std::locale &loc)
    {
        return to_wstring(str.data(), str.size(), loc);
    }

    std::wstring to_wstring(const char *from, std::size_t size, const std::locale &loc)
    {
        auto &f = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(loc);

        std::mbstate_t mb;
        std::wstring to(size, '\0');
        const char *from_next = from;
        wchar_t *to_next = to.data();

        std::codecvt_base::result result = f.in(
            mb,                     // state
            from,                   // from
            from + size,            // from_end
            from_next,              // from_next
            to.data(),              // to
            to.data() + to.size(),  // to_end
            to_next);               // to_next

        if (result == std::codecvt_base::result::ok)
        {
            to.resize(to_next - to.data());
            return to;
        }
        else
        {
            throw std::invalid_argument(
                "Invalid " +
                loc.name() +
                " character in input string" +
                std::to_string(from_next - from));
        }
        // static std::wstring_convert<std::codecvt<wchar_t, char, std::mbstate_t>> conv;
        // return conv.from_bytes(from, from+size);
    }

    std::string wrap(const std::vector<std::string> &words,
                     size_t start_column,
                     size_t left_margin,
                     size_t right_margin)
    {
        std::stringstream ss;
        size_t current_column = start_column;
        uint spaces = 0;

        for (const std::string &word : words)
        {
            if (current_column > left_margin &&
                current_column + spaces + word.length() > right_margin)
            {
                ss << std::endl;
                current_column = 0;
                spaces = 0;
            }
            if (current_column < left_margin)
            {
                ss << std::string(left_margin - current_column, ' ');
                current_column = left_margin;
                spaces = 0;
            }
            if (spaces)
            {
                ss << std::string(spaces, ' ');
            }
            ss << word;
            current_column += spaces + (uint)word.length();
            spaces = 1;
        }
        ss << std::endl;
        return ss.str();
    }

    std::string wrap(const std::string &input,
                     size_t start_column,
                     size_t left_margin,
                     size_t right_margin,
                     bool keep_empties)
    {
        std::stringstream ss;

        for (const std::string &line : split(input, NEWLINE, 0, true))
        {
            ss << wrap(split(line, " ", 0, keep_empties),
                       start_column,
                       left_margin,
                       right_margin);
            start_column = 0;
        }
        return ss.str();
    }

    std::vector<std::string> split(const std::string &string,
                                   const std::string &delimiter,
                                   uint maxsplits,
                                   bool keep_empties)
    {
        std::vector<std::string> parts;
        if (maxsplits > 0)
        {
            parts.reserve(maxsplits + 1);
        }

        size_t pos = 0;
        size_t end = string.find(delimiter);
        uint splits = 0;

        while ((end != std::string::npos) &&
               (maxsplits == 0 || splits < maxsplits))
        {
            if (keep_empties || end > pos)
            {
                parts.push_back(string.substr(pos, end - pos));
                splits++;
            }
            pos = end + delimiter.length();
            end = string.find(delimiter, pos);
        }
        if (keep_empties || string.length() > pos)
        {
            parts.push_back(string.substr(pos));
        }
        return parts;
    }

    std::vector<std::string> splitlines(
        const std::string &string,
        uint maxsplits,
        bool keep_empties)
    {
        std::vector<std::string> parts;
        size_t start = 0;
        uint splits = 0;
        const char *ch = string.data();
        char last = '\0';

        for (size_t pos = 0;
             (pos < string.length()) && (!maxsplits || (splits < maxsplits));
             ch++, pos++)
        {
            switch (*ch)
            {
            case '\r':
            case '\n':
                if ((*ch == '\r') || (last != '\r'))
                {
                    if (keep_empties || (pos > start))
                    {
                        parts.push_back(string.substr(start, pos - start));
                        splits++;
                    }
                }
                start = pos + 1;
                break;
            }
            last = *ch;
        }

        if (keep_empties || (string.length() > start))
        {
            parts.push_back(string.substr(start));
        }

        return parts;
    }

    std::string join(const std::vector<std::string> &vector,

                     const std::string &delimiter,
                     bool keep_empties,
                     bool quoted)
    {
        return join(vector.begin(), vector.end(), delimiter, keep_empties, quoted);
    }

    std::string quoted(const std::string &input)
    {
        std::stringstream out;
        out << std::quoted(input);
        return out.str();
    }

    std::string unquoted(const std::string &input)
    {
        std::string out;
        std::stringstream(input) >> std::quoted(out);
        return out;
    }

    std::string escaped(
        const std::string &input,
        const std::unordered_set<char> &extra_escapes)
    {
        std::stringstream out;
        escape(out, input, extra_escapes);
        return out.str();
    }

    void escape(
        std::ostream &out,
        const std::string_view &input,
        const std::unordered_set<char> &extra_escapes)
    {
        static const std::unordered_map<char, std::string> escape_map = {
            {'\0', "\\0"},
            {'\\', "\\\\"},
            {'\a', "\\a"},
            {'\b', "\\b"},
            {'\f', "\\f"},
            {'\n', "\\n"},
            {'\r', "\\r"},
            {'\t', "\\t"},
            {'\v', "\\v"},
            {'\x7f', "\\x7f"},
        };

        std::ios_base::fmtflags original_flags = out.flags();
        out << std::hex;

        for (auto it = input.begin(); it != input.end(); it++)
        {
            if (auto it2 = escape_map.find(*it); it2 != escape_map.end())
            {
                out << it2->second;
            }
            else if (*it < ' ')
            {
                out << "\\x" << static_cast<std::uint8_t>(*it);
            }
            else if (extra_escapes.count(*it))
            {
                out << "\\" << *it;
            }
            else
            {
                out << *it;
            }
        }
        out.flags(original_flags);
    }

    std::string unescaped(const std::string &input)
    {
        std::stringstream out;
        unescape(out, input);
        return out.str();
    }

    void unescape(std::ostream &out,
                  const std::string_view &input)
    {
        std::stringstream ord;
        std::size_t digitsRemaining = 0;
        bool escaped = false;

        for (auto it = input.begin(); it < input.end(); it++)
        {
            if (escaped)
            {
                switch (*it)
                {
                case 'a':
                    out << "\a";
                    break;
                case 'e':
                    out << '\x1b';
                    break;
                case 'f':
                    out << "\f";
                    break;
                case 'n':
                    out << "\n";
                    break;
                case 'r':
                    out << "\r";
                    break;
                case 't':
                    out << "\t";
                    break;
                case 'v':
                    out << "\v";
                    break;

                case 'x':
                case 'X':
                    digitsRemaining = 2;
                    ord << std::hex;
                    break;

                case 'u':
                    digitsRemaining = 4;
                    ord << std::hex;
                    break;

                case 'U':
                    digitsRemaining = 8;
                    ord << std::hex;
                    break;

                default:
                    out << *it;
                    break;
                }
                escaped = false;
            }
            else if (digitsRemaining > 0)
            {
                ord << *it;
                if (--digitsRemaining % 2 == 0)
                {
                    std::uint8_t value;
                    ord >> value;
                    out << (char)value;
                }
            }
            else if (*it == '\\')
            {
                escaped = true;
            }
            else
            {
                out << *it;
            }
        }

        if (escaped)
        {
            //throw(std::runtime_error("Character escape at end of string"));
            out.setstate(std::ios::failbit);
        }
    }

    /// Escape and quote a string
    std::string to_literal(
        const std::string &input)
    {
        std::stringstream out;
        to_literal(out, input);
        return out.str();
    }

    void to_literal(
        std::ostream &out,
        const std::string_view &input)
    {
        out << '"';
        escape(out, input, {'"'});
        out << '"';
    }

    /// Escape and quote a string
    std::string from_literal(
        const std::string &input)
    {
        std::stringstream out;
        from_literal(out, input);
        return out.str();
    }

    void from_literal(
        std::ostream &out,
        const std::string_view &input)
    {
        const char *begin = input.data();
        const char *end = begin + input.size();

        if ((begin != end) && (*begin == '"'))
        {
            begin++;
        }

        if ((begin != end) && (*(end - 1) == '"'))
        {
            end--;
        }

        std::string_view::size_type size = (end - begin);
        unescape(out, {begin, size});
    }

    std::string url_decoded(
        const std::string &encoded_url)
    {
        std::string output;
        output.reserve(encoded_url.size());

        std::string escape;
        for (char c : encoded_url)
        {
            if (escape.empty())
            {
                if (c != '%')
                {
                    output += c;
                }
                else
                {
                    escape = c;
                }
            }
            else if (std::isxdigit(c))
            {
                escape += c;
                if (escape.size() == 3)
                {
                    unsigned long byte_value = stoul(escape.substr(1), nullptr, 16);
                    output += static_cast<char>(byte_value);
                    escape.clear();
                }
            }
            else
            {
                output += escape;
                escape.clear();
            }
        }
        output.shrink_to_fit();
        return output;
    }

    std::size_t substitute(const std::string &original,
                           const std::string &replacement,
                           std::string *string)
    {
        std::size_t substitutions = 0;
        if (original.length() > 0)
        {
            std::string::size_type pos = string->find(original);
            while (pos != std::string::npos)
            {
                string->replace(pos, original.length(), replacement);
                pos = string->find(original, pos + replacement.length());
                substitutions++;
            }
        }
        return substitutions;
    }

    bool startswith(const std::string &input,
                    const std::string &substring,
                    bool ignore_case)
    {
        if (ignore_case)
        {
            return tolower(input).compare(0, substring.length(), tolower(substring)) == 0;
        }
        else
        {
            return input.compare(0, substring.length(), substring) == 0;
        }
    }

    bool endswith(const std::string &input,
                  const std::string &substring,
                  bool ignore_case)
    {
        if (input.length() < substring.length())
        {
            return false;
        }
        else if (ignore_case)
        {
            return tolower(input).compare(input.length() - substring.length(),
                                          substring.length(),
                                          tolower(substring)) == 0;
        }
        else
        {
            return input.compare(input.length() - substring.length(),
                                 substring.length(),
                                 substring) == 0;
        }
    }

    bool is_valid_symbol(const std::string &input)
    {
        return (
            (input.size() > 0) &&
            std::isalpha(static_cast<unsigned char>(input.front())) &&
            std::all_of(
                input.begin(),
                input.end(),
                [](char c) -> bool {
                    return std::isalnum(static_cast<unsigned char>(c)) || (c == '_');
                }));
    }

    bool is_unicode_text(const std::string &input)
    {
        return (input.compare(0, UNICODE_BOM.length(), UNICODE_BOM) == 0);
    }

    std::string to_unicode_text(
        const std::string &input)
    {
        return is_unicode_text(input)
                   ? input
                   : (UNICODE_BOM + input);
    }

    std::string stem(const std::string &string,
                     const std::string &separator)
    {
        if (std::string::size_type last = string.rfind(separator);
            last != std::string::npos)
        {
            return string.substr(last + 1);
        }
        else
        {
            return string;
        }
    }

    std::string strip(const std::string &string,
                      const std::set<char> &lstrip,
                      const std::set<char> &rstrip)
    {
        const char *start = string.data();
        const char *end = string.data() + string.size();

        while (start != end && lstrip.count(*start) > 0)
        {
            start++;
        }

        while (end != start && rstrip.count(*(end - 1)) > 0)
        {
            end--;
        }

        return std::string(start, end - start);
    }

    std::string common_prefix(
        const std::vector<std::string> &strings)
    {
        for (uint pos = 0;; pos++)
        {
            char common_char = '\0';
            for (const std::string &s : strings)
            {
                if ((s.length() <= pos) || (common_char && (common_char != s.at(pos))))
                {
                    return s.substr(0, pos);
                }
                else
                {
                    common_char = s.at(pos);
                }
            }
        }
        return "";
    }

}  // namespace core::str
