/// -*- c++ -*-
//==============================================================================
/// @file format.h++
/// @brief String formatting from template string, using C++ output streams
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "types/streamable.h++"

#include <iomanip>
#include <ostream>
#include <regex>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <cmath>
#include <chrono>
#include <optional>
#include <sstream>

using ulong = unsigned long;
using uint = unsigned int;
using namespace std::literals::string_literals;  // ""s

namespace core::str
{
    /// \class Formatter
    /// \brief
    ///   Helper class for printf-style string formatting from template string,
    ///   using C++ output streams.
    ///
    /// \sa str::format(), below.

    class Formatter
    {
    public:
        Formatter(std::ostream &stream, const std::string &format);

        template <class... Args>
        inline Formatter &add(const Args &...args) noexcept
        {
            (this->append(args), ...);
            return *this;
        }

        template <class T>
        inline void append(const T &value) noexcept
        {
            if (this->parts_it != this->parts.end())
            {
                const Part &part = *this->parts_it++;
                this->stream.imbue(std::locale::classic());
                std::ios_base::fmtflags original_flags = this->stream.flags();
                const Modifiers &modifiers = this->apply_format(part, sizeof(T));
                this->appendvalue(value, modifiers);
                this->stream.flags(original_flags);
                this->stream << part.tail;
            }
        }

        void check() const;
        void add_tail() const;

    private:
        struct Part
        {
            Part(char conversion = '\0',
                 ulong pos = 0,
                 const std::string &flags = "",
                 const std::optional<uint> &width = {},
                 const std::optional<uint> &precision = {},
                 const std::string &tail = "")
                : conversion(conversion),
                  pos(pos),
                  flags(flags),
                  width(width),
                  precision(precision),
                  tail(tail)
            {
            }

            char conversion;
            ulong pos;
            std::string flags;
            std::optional<uint> width;
            std::optional<uint> precision;
            std::string tail;
        };

        using Parts = std::vector<Part>;

        struct Modifiers
        {
            bool ignore = false;
            bool alternate = false;
            bool quoted = false;
            bool varwidth = false;
            bool lower = false;
            bool upper = false;
            bool signspace = false;
            bool saveargs = false;
            bool shortform = false;
            bool hidden = false;
            bool truncate = false;
            bool nonegativezero = false;
            char timeformat = '\0';
        };

        Parts split_parts(const std::string &fmt) const;
        std::optional<uint> optional_size(const std::string &size) const;

        Modifiers apply_format(const Part &part, uint bytesize);
        void apply_flags(const std::string &flagstring, Modifiers *modifiers);
        void apply_conversion(char conv, uint bytesize, Modifiers *modifiers);
        std::string nonarg_conversion(char conversion) const;

        // Ouptut formatting for boolean values (to support quoting, lower/upper)
        void appendvalue(bool value,
                         const Modifiers &modifiers);

        // Ouptut formatting for char (to support quoting, lower/upper)
        void appendvalue(char value,
                         const Modifiers &modifiers);

        // Ouptut formatting for character arrays (copied if needed)
        void appendvalue(const char *value,
                         const Modifiers &modifiers);

        // Ouptut formatting for std::string_view (to support quoting)
        void appendvalue(const std::string_view &value,
                         const Modifiers &modifiers);

        // Ouptut formatting for lvalue strings (copied if needed)
        void appendvalue(const std::string &value,
                         const Modifiers &modifiers);

        // Ouptut formatting for rvalue strings (destrutive lower/upper case conversion)
        void appendvalue(std::string &&rvalue,
                         const Modifiers &modifiers);

        // Ouptut formatting for timepionts
        void appendvalue(const std::chrono::system_clock::time_point &tp,
                         const Modifiers &modifiers);

        void appendvalue(const std::tm &time,
                         const Modifiers &modifiers);

        // // Ouptut formatting for byte arrays (to support quoting)
        // void appendvalue(const std::vector<std::uint8_t> &value,
        //                  const Modifiers &modifiers);

        // Output formatting for integer pointers, to support `%n` format
        void appendvalue(int *nargs,
                         const Modifiers &modifiers);

        void appendvalue(const types::Streamable &value,
                         const Modifiers &modifiers);

        // Output formatting for integral types: short, ushort, int, uint...
        // to support (a) variable field length and (b) sign space
        template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
        inline void appendvalue(const T &value,
                                const Modifiers &modifiers)
        {
            // Variable width argument (`%*`), no conversion
            if (modifiers.varwidth)
            {
                this->varwidth = static_cast<uint>(value);
            }
            else
            {
                if (modifiers.signspace && (value >= 0))
                    this->stream << " ";

                // Unary +  to ensure numeric output of `signed char`/`unsigned char`.
                // Plain `char` are not affected as std::is_integral_t<char> is false.
                this->stream << +value;
            }
        }

        // Output formatting for floating point types: float, double,
        // to support 'z' format to generate truncated values
        template <class T, std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
        inline void appendvalue(const T &value,
                                const Modifiers &modifiers)
        {
            // If the value is negative but close enough to zero that
            // the string representation is all zeroes, and if no minus
            // sign is desired in this case, just use the value 0.0.
            if (modifiers.nonegativezero &&
                (value < 0.0f) &&
                (-std::log10(-value) >= stream.precision()))
            {
                this->stream << 0.0;
            }
            else if (modifiers.truncate)
            {
                this->stream << std::trunc(value);
            }
            else
            {
                this->stream << value;
            }
        }

        // Output formatting for non-numeric types, except as overloaded above
        template <class T,
                  std::enable_if_t<!std::is_arithmetic_v<T> && !std::is_base_of_v<types::Streamable, T>,
                                   bool> = true>
        inline void appendvalue(const T &value,
                                const Modifiers &)
        {
            this->stream << value;
        }

    private:
        std::ostream &stream;
        std::locale locale;
        const std::string formatstring;
        const Parts parts;
        Parts::const_iterator parts_it;
        std::optional<uint> varwidth;
    };

    /// \fn format
    /// \brief
    ///     Generate printf()-style formatted string, using C++ output stream.
    /// \param[in] format
    ///     String with zero or more format specifiers corresponding to
    ///     subsequent arguments.
    /// \param[in] args
    ///     Argument corresponding to the placeholders in the format string.
    ///
    /// Each format specifier contains the following components, in order:
    ///   * A percent symbol, `%`
    ///   * Zero or more flag characters to adjust the formatting of some types
    ///   * Field width specifier (optional)
    ///   * A period followed by precision (optional)
    ///   * Length modifier (optional, ignored).
    ///   * Conversion type.
    ///
    /// Only the percent symbol and the conversion type are required.
    ///
    /// Note that the specifiers do not specify any typecasting as is the case
    /// with printf(); in fact, it would be impossible to represent all possible
    /// argument types this way. (For instance, printf() does not even provide
    /// any format code for std::string, let alone arbitrary C++ types with "<<"
    /// operators).  Instead, they are used to trigger corresponding output
    /// stream manipulators (such as std::hex, std::setw(), std::setprecision())
    /// or other formatting modifications (like std::quoted(), std::tolower()).
    ///
    /// These are the supported flag characters:
    ///   * `#` (hash) alternate form; mainly sets `std::showbase`
    ///   * `0` (zero) sets `std::setfill('0')` and `std::internal`
    ///   * `-` (minus) sets `std::left`
    ///   * '+' (plus) sets `std::showpos`
    ///   * ` ` (space) inserts a space if the argument is a nonnegative integer
    ///   * `'` (single quote) sets locale in order to apply thousands separator
    ///   * `^` (caret) converts a string or character argument to uppercase
    ///   * `,` (comma) converts a string or character argument to lowercase
    ///
    /// If present, the field width specifier is either one or more digits or
    /// the character `*`, and specifies the minimum length of the formatted
    /// output value.  In the latter case, the field with should be specified in
    /// an additional argument prior to the actual value, e.g., (`"%*s", 10,
    /// "Hello"`).  In either case, the resulting value is applied via
    /// `std::setw(N)`.
    ///
    /// Similarly, if a precision specifier (`.M`) is present, it specifies the
    /// number of significant digits in a floating point value, and is applied
    /// via `std::setprecision(M)`.
    ///
    /// Any length modifier `hh`, `h`, `ll`, `l`, `q`, `L`, `j` , `z`, `Z`, `t`
    /// is silently absorbed and ignored for printf() compatibility.
    ///
    /// The following conversion types are supported; those that are additions
    /// or modifications from `printf()` are marked with [*]:
    ///  `d`, `i`, `u`  sets std::dec to apply decimal integer representation.
    ///                 with alternate flag apply locale-specific grouping.
    ///  `z` [*]        truncates a floating point value and represents as integer;
    ///                 with alternate flag, apply locale-specific grouping.
    ///  `n` [*]        apply locale-specific grouping (e.g. thousands separator) to numbers
    ///  `o`            sets std::oct to apply octal integer representation.
    ///                 With alternate flag, include leading `0`.
    ///  `x`, `X`       sets std::hex to apply hexadecimal representation
    ///                 With alternate flag, include leading `0x` or `0X`.
    ///  `f`            sets std::fixed floating point representation
    ///  `F` [*]        sets std::fixed while eliminating "negative zero" values
    ///  `e`, `E`       sets std::scientific floating point representation
    ///  `g`, `G`       sets std::defaultfloat floating point representation
    ///  `a`, `A`       sets std::hexfloat floating point representation
    ///  `c`, `s`       no modifications; intended for char and std::string
    ///  `O` [*]        no modifications; intended for arbitrary C++ objects
    ///  `h` [*]        hide characters in string by replacing each with `*`
    ///  `r` [*]        applies `literal` format, e.g., std::quoted() around string arg.
    ///  `b` [*]        sets std::boolalpha to show boolean values as "false"/"true"
    ///  `T` [*]        timepoint as a ISO 8601 string with `T` as date/time separator,
    ///                 representing local date/time.
    ///  `Z` [*]        timepoint as a JavaScript time string (UTC/Zulu with `Z` suffix),
    ///                 representing UTC date/time.
    ///  `p`            pointer format: std::hex, std::showbase, std::internal,
    ///                 std::setw(2+sizeof(void*)*2), std::setfill(`0`)
    ///
    /// Additionally, the uppercase variants `X`, `E`, `G`, and `A` applies
    /// `std::uppercase`.
    ///
    /// As a special case, `%#0x` and `%0x` (zero-padded hexadecimal format with
    /// no explicit field length, with or without base shown) pads extra zeroes
    /// as appropriate for the specific integral type of the argument. For
    /// instance, the 32-bit integer 42 would be represented as `0x0000002a` and
    /// `0000002a`, respectively.

    template <class... Args>
    [[nodiscard]] inline std::string format(const std::string &format,
                                            const Args &...args) noexcept
    {
        std::ostringstream stream;
        Formatter(stream, format).add(args...).add_tail();
        return stream.str();
    }

    template <class... Args>
    inline std::ostream &format(std::ostream &stream,
                                const std::string &format,
                                const Args &...args) noexcept
    {
        Formatter(stream, format).add(args...).add_tail();
        return stream;
    }
}  // namespace core::str

using core::str::format;
