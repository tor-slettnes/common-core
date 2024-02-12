/// -*- c++ -*-
//==============================================================================
/// @file format.c++
/// @brief String formatting from template string, using C++ output streams
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/format.h++"

#include <string.h>

#include <cstdint>
#include <iomanip>  // output stream format manipulations
#include <ios>      // left, right, internal

namespace shared::str
{
    constexpr auto VARARG_CONVERSION = '*';

    Formatter::Formatter(std::ostream &stream, const std::string &format)
        : stream(stream),
          formatstring(format),
          parts(this->split_parts(format)),
          parts_it(parts.begin())
    {
        this->stream << (parts_it++)->tail;
    }

    Formatter::Parts Formatter::split_parts(const std::string &fmt) const
    {
        static const std::regex rx(
            //"(?:^|[^\\\\])(?:\\\\{2})*"      // 0 or even # of preceeding backslashes
            "(%"                               // (1) Entire expression starting with %
            "([#0\\-\\ \\+\\'\\^,]*)"          // (2) 0 or more flags #|0|-| |+|'|^|,
            "(?:(\\d*)|(\\*))"                 // (3) fixed or (4) variable field width
            "(?:\\.(\\d*))?"                   // (5) precision
            "(hh|h|ll|l|q|L|j|z|Z|t)?"         // (6) length modifier (ignored)
            "(?:([abcdefghinoprsuxzAEFGOX])|"  // (7) argument conversion specifier, or
            "([m%])))"                         // (8) non-argument specifier
        );
        uint pos = 0, next = 0;

        Parts parts;
        std::string flags;
        std::optional<uint> width, precision;
        char conversion = '\0';
        std::string tail;

        auto matchit = std::sregex_iterator(fmt.begin(), fmt.end(), rx);
        for (std::sregex_iterator endit; matchit != endit; matchit++)
        {
            pos = (uint)matchit->position(1);
            // Absorb the literal portion of the format string since the last
            // specifier
            tail.append(fmt.substr(next, pos - next));

            if (matchit->length(8))  // Non-argument format (e.g. %%).
            {
                // Absorb the corresponding non-argument conversion, and move on
                tail.append(this->nonarg_conversion(matchit->str(8)[0]));
            }
            else
            {
                // Add the literal portion of the format string leading up to
                // this specifier
                parts.emplace_back(conversion, pos, flags, width, precision, tail);
                tail.clear();

                // Determine format of the next part.
                flags = matchit->str(2);
                width = this->optional_size(matchit->str(3));
                precision = this->optional_size(matchit->str(5));
                conversion = matchit->str(7)[0];

                if (matchit->length(4))  // Variable field width specifier
                {
                    // Insert an "empty" part for a corresponding field width
                    // argument
                    parts.emplace_back(VARARG_CONVERSION, pos);
                }
            }

            next = (uint)matchit->position(1) + (uint)matchit->length(1);
        }

        tail.append(fmt.substr(next));
        parts.emplace_back(conversion, pos, flags, width, precision, tail);
        return parts;
    }

    Formatter::Modifiers Formatter::apply_format(const Part &part, uint bytesize)
    {
        Formatter::Modifiers mods;
        this->stream.imbue(std::locale("C"));
        this->apply_flags(part.flags, &mods);

        if (part.width)
        {
            this->stream << std::setw(part.width.value());
        }
        else if (this->varwidth)
        {
            this->stream << std::setw(this->varwidth.value());
        }
        this->varwidth.reset();

        if (part.precision)
        {
            this->stream << std::setprecision(part.precision.value());
        }
        this->apply_conversion(part.conversion, bytesize, &mods);
        return mods;
    }

    void Formatter::apply_flags(const std::string &flagstring, Formatter::Modifiers *modifiers)
    {
        for (char flag : flagstring)
        {
            switch (flag)
            {
            case '#':
                modifiers->alternate = true;
                break;

            case '0':
                // Pad with zeroes, between prefix (e.g. 0x) and value.
                this->stream << std::setfill('0') << std::internal;
                break;

            case '-':
                // Left-align
                this->stream << std::left;
                break;

            case ' ':
                // ignored
                modifiers->signspace = true;
                break;

            case '+':
                this->stream << std::showpos;
                break;

            case '\'':
                // Apply current locale to output stream, including thousands separator
                this->stream.imbue(std::locale());
                break;

            case '^':
                modifiers->upper = true;
                break;

            case ',':
                modifiers->lower = true;
                break;

            default:
                // assertf(false, "Unknown flag %r in format string %r", flag, this->formatstring);
                break;
            }
        }
    }

    void Formatter::apply_conversion(char conversion,
                                     uint bytesize,
                                     Formatter::Modifiers *modifiers)
    {
        switch (conversion)
        {
        case 'b':
            this->stream << std::boolalpha;
            break;

        case 'c':
            modifiers->shortform = true;
            break;

        case 's':
        case 'O':
            this->stream << std::boolalpha;
            break;

        case 'd':
        case 'i':
        case 'u':
        case 'z':
            // Decimal integer representation
            this->stream << std::dec << std::fixed << std::setprecision(0);
            modifiers->truncate = (conversion == 'z');
            break;

        case 'o':
            // Octal integer representation
            this->stream << std::oct;
            if (modifiers->alternate)
            {
                this->stream << std::showbase << std::setprecision(0);
            }
            break;

        case 'x':
        case 'X':
            // Hexadecimal integer representation
            this->stream << std::hex;
            if (modifiers->alternate)
            {
                this->stream << std::showbase << std::setprecision(0);
            }
            if ((this->stream.fill() == '0') && (this->stream.width() == 0))
            {
                this->stream << std::setw((modifiers->alternate ? 2 : 0) + bytesize * 2)
                             << std::setprecision(0);
            }
            break;

        case 'e':
        case 'E':
            this->stream << std::scientific;
            break;

        case 'f':
            this->stream << std::fixed;
            break;

        case 'F':
            modifiers->nonegativezero = true;
            this->stream << std::fixed;
            break;

        case 'g':
        case 'G':
            this->stream << std::defaultfloat;
            break;

        case 'a':
        case 'A':
            this->stream << std::hexfloat;
            break;

        case 'r':
            modifiers->quoted = true;
            this->stream << std::boolalpha;
            break;

        case 'p':
            this->stream << std::setw(2 + sizeof(void *) * 2) << std::setfill('0')
                         << std::showbase << std::internal << std::hex;
            break;

        case 'n':
            // The argument should be a pointer to an int, which will receive argument count
            modifiers->saveargs = true;
            break;

        case 'h':
            modifiers->hidden = true;
            break;

        case VARARG_CONVERSION:
            // The argument should be an integer, which specifies the field width
            modifiers->varwidth = true;
            break;

        case '\0':
            // Empty part
            break;

        default:
            // assertf(false,
            //         "Unhandled format character %r in string %r",
            //         conversion,
            //         this->formatstring);
            break;
        }

        switch (conversion)
        {
        case 'X':
        case 'E':
        case 'F':
        case 'G':
        case 'A':
            this->stream << std::uppercase;
            break;

        default:
            this->stream << std::nouppercase;
            break;
        }
    }

    std::string Formatter::nonarg_conversion(char conversion) const
    {
        switch (conversion)
        {
        case 'm':
            return strerror(errno);

        case '%':
            return "%";

        default:
            // assertf(false,
            //         "Unhandled format character %r in format string %r",
            //         conversion, this->formatstring);
            return "";
        }
    }

    std::optional<uint> Formatter::optional_size(const std::string &size) const
    {
        if (size.length())
        {
            return std::stoul(size);
        }
        else
        {
            return {};
        }
    }

    void Formatter::appendvalue(bool value,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.shortform)
        {
            this->stream << (value ? 't' : 'f');
        }
        else
        {
            this->stream << value;
        }
    }

    void Formatter::appendvalue(char value,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.lower)
            value = std::tolower(value);
        else if (modifiers.upper)
            value = std::toupper(value);

        if (modifiers.quoted)
            this->stream << "'" << value << "'";
        else
            this->stream << value;
    }

    void Formatter::appendvalue(const std::string_view &value,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.upper || modifiers.lower || modifiers.hidden)
        {
            this->appendvalue(std::string(value), modifiers);
        }
        else if (modifiers.quoted)
        {
            this->stream << std::quoted(value);
        }
        else
        {
            this->stream << value;
        }
    }

    void Formatter::appendvalue(const std::string &value,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.upper || modifiers.lower || modifiers.hidden)
        {
            // String needs modification. Reinvoke with an rvalue copy.
            this->appendvalue(std::string(value), modifiers);
        }
        else if (modifiers.quoted)
        {
            this->stream << std::quoted(value);
        }
        else
        {
            this->stream << value;
        }
    }

    void Formatter::appendvalue(std::string &&rvalue,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.hidden)
        {
            for (auto &c : rvalue) c = '*';
        }
        else if (modifiers.lower)
        {
            for (auto &c : rvalue) c = std::tolower(c);
        }
        else if (modifiers.upper)
        {
            for (auto &c : rvalue) c = std::toupper(c);
        }

        if (modifiers.quoted)
        {
            this->stream << std::quoted(rvalue);
        }
        else
        {
            this->stream << rvalue;
        }
    }

    // void Formatter::appendvalue(const std::vector<std::uint8_t> &value,
    //                             const Modifiers &modifiers)
    // {
    //     return this->appendvalue(
    //         std::string_view(reinterpret_cast<const char *>(value.data()), value.size()),
    //         modifiers);
    // }

    void Formatter::appendvalue(int *nargs,
                                const Formatter::Modifiers &modifiers)
    {
        if (modifiers.saveargs)  // %n format.
        {
            *nargs = static_cast<int>(std::distance(this->parts.begin(), this->parts_it));
        }
        else
        {
            this->appendvalue((void *)nargs, modifiers);
        }
    }

    void Formatter::appendvalue(const types::Streamable &value,
                                const Modifiers &modifiers)
    {
        if (modifiers.quoted)
        {
            value.to_literal_stream(this->stream);
        }
        else
        {
            value.to_stream(this->stream);
        }
    }

    void Formatter::check() const
    {
        // assertf(this->argc == this->parts.size(),
        //         "Incorrect number of arguments for format string %r (got %d, expected %d)",
        //         this->formatstring,
        //         this->argc,
        //         this->parts.size());
    }

    void Formatter::add_tail() const
    {
        if (this->parts_it != this->parts.end())
        {
            this->stream << this->formatstring.substr(this->parts_it->pos);
        }
    }

}  // namespace shared::str
