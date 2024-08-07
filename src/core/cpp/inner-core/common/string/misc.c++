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

    std::string toupper(const std::string &s, const std::locale &loc)
    {
        std::string result(s);
        toupper(&result, loc);
        return result;
    }

    std::string tolower(const std::string &s, const std::locale &loc)
    {
        std::string result(s);
        tolower(&result, loc);
        return result;
    }

    std::string obfuscated(const std::string &s)
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

    // std::string unquoted (const std::string &input, const std::string &openquote, const std::string &closequote)
    // {
    //     std::string::size_type quote_length = openquote.length() + closequote.length();
    //     if (input.length() >= quote_length  &&
    //         input.compare(openquote) == 0 &&
    //         input.compare(input.length()-closequote.length(), closequote.length(), closequote) == 0)
    //     {
    //         return unescaped(input.substr(openquote.length(), input.length()-quote_length));
    //     }
    //     else
    //     {
    //         throw(std::runtime_error("Missing quotation mark(s)"));
    //     }
    // }

    std::string unescaped(const std::string &input)
    {
        std::stringstream out, ord;
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
            throw(std::runtime_error("Character escape at end of string"));
        }
        return out.str();
    }

    void substitute(const std::string &original,
                    const std::string &replacement,
                    std::string *string)
    {
        if (original.length() > 0)
        {
            std::string::size_type pos = string->find(original);
            while (pos != std::string::npos)
            {
                string->replace(pos, original.length(), replacement);
                pos = string->find(original, pos + replacement.length());
            }
        }
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
