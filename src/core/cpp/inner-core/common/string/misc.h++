/// -*- c++ -*-
//==============================================================================
/// @file misc.h++
/// @brief Miscellaneous string manipulations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "stream.h++"

#include <iomanip>
#include <iostream>
#include <locale>
#include <regex>
#include <string>
#include <typeinfo>
#include <type_traits>  // std::is_same_v<T, U>
#include <unordered_map>
#include <vector>
#include <set>

#define WHITESPACE                       \
    {                                    \
        ' ', '\t', '\r', '\n', 'f', '\0' \
    }

std::string operator""_u(const char *str, std::size_t len);

/// Miscellaneous string manipulations
namespace core::str
{
    //    constexpr auto WHITESPACE = { ' ', '\t', '\r', '\n', 'f', '\0' };

#if defined(_WIN32) || defined(_WIN64)
    constexpr auto NEWLINE = "\r\n";
#else
    constexpr auto NEWLINE = "\n";
#endif

    inline const std::string UNICODE_BOM = "\uFEFF";


    /// Convert string to uppercase in place, using the specified locale or current locale.
    /// @param[in,out] s String to convert.
    /// @param[in] loc Locale. Defaults to global locale.
    void toupper(
        std::string *s,
        const std::locale &loc = {});

    /// Convert string to lowercase, using the specified locale or current locale.
    /// @param[in] s
    ///     Original string
    /// @param[in] loc
    ///     Locale. Defaults to global locale.
    void tolower(
        std::string *s,
        const std::locale &loc = {});

    /// Convert string to uppercase, using the specified locale or current locale.
    /// @param[in] s
    ///     Original string
    /// @param[in] loc
    ///     Locale. Defaults to global locale.
    /// @return Copy of string converted to uppercsae.
    [[nodiscard]] std::string toupper(
        std::string s,
        const std::locale &loc = {});

    /// @brief Convert string to lowercase in place, using the specified locale or current locale.
    /// @param[in,out] s
    ///      String to convert.
    /// @param[in] loc
    ///      Locale. Defaults to global locale.
    /// @return Copy of string converted to lowercase.
    [[nodiscard]] std::string tolower(
        std::string s,
        const std::locale &loc = {});

    /// @brief Convert from (UTF8-encoded) `std::string` to (UTF16 or UCS2-encoded) `std::wstring`.
    /// @param[in] wstr
    ///      Wide string
    /// @param[in] loc
    ///      Locale
    /// @return
    ///      Byte string
    [[nodiscard]] std::string from_wstring(
        const std::wstring &wstr,
        const std::locale &loc = {});

    [[nodiscard]] std::string from_wstring(
        const wchar_t *begin,
        std::size_t size,
        const std::locale &loc = {});

    /// @brief Convert from `std::wstring` to `std::string`
    /// @param[in] str
    ///      ByteWide string
    /// @param[in] loc
    ///      Locale
    /// @return
    ///      Wide string
    [[nodiscard]] std::wstring to_wstring(
        const std::string &str,
        const std::locale &loc = {});

    [[nodiscard]] std::wstring to_wstring(
        const char *begin,
        std::size_t size,
        const std::locale &loc = {});

    /// @brief Return representation of a string with characters replaced by asterisks
    /// @param[in] s
    ///     Input string
    /// @return
    ///     String with characters replaced by asterisks
    [[nodiscard]] std::string obfuscated(
        const std::string_view &s);

    /// Return a universally unique identifier (UUID)
    // std::string uuid();

    /// Wrap words in a string to fit between left and right margins.
    /// @param[in] input
    ///     Input string
    /// @param[in] start_column
    ///     Current output column, subtraced from the left margin of the first line
    /// @param[in] left_margin
    ///     Number of whitespaces to insert at the beginning of each new line.
    /// @param[in] right_margin
    ///     Wrap column. Output lines MAY exceed this length if a single word cannot
    ///     otherwise fit on one line.
    /// @param[in] keep_empties
    ///     Include empty strings in case of successive spaces in input string
    /// @return
    ///     Output string.

    [[nodiscard]] std::string wrap(
        const std::string &input,
        size_t start_column = 0,
        size_t left_margin = 24,
        size_t right_margin = 80,
        bool keep_empties = false);

    /// @fn wrap
    /// @brief Wrap words to fit between left and right margins.
    /// @param[in] words
    ///     Vector of words to wrap
    /// @param[in] start_column
    ///     Current output column, subtraced from the left margin of the first line
    /// @param[in] left_margin
    ///     Number of whitespaces to insert at the beginning of each new line.
    /// @param[in] right_margin
    ///     Wrap column. Output lines MAY exceed this length if a single word cannot
    ///     otherwise fit on one line.
    /// @return
    ///     Output string.

    [[nodiscard]] std::string wrap(
        const std::vector<std::string> &words,
        size_t start_column = 0,
        size_t left_margin = 24,
        size_t right_margin = 80);

    /// @fn split
    /// @brief
    ///     Separate a string at each occurence of \p delimiter into one or more
    ///     substrings.
    /// @param[in] string
    ///     Original string
    /// @param[in] delimiter
    ///     Separator by which to split the input string
    /// @param[in] keep_empties
    ///     Keep empty substrings from input string.
    ///     This guarantess at least one element in the response.
    /// @param[in] maxsplits
    ///     If non-zero, split at most this number of times
    ///     (returning no more than `maxsplit+1` substrings)
    /// @return
    ///     List of one or more non-overlapping substrings of \p string
    ///     separated by \p delimiter

    [[nodiscard]] std::vector<std::string> split(
        const std::string &string,
        const std::string &delimiter = " ",
        uint maxsplits = 0,
        bool keep_empties = false);

    /// @fn splitlines
    /// @brief
    ///     Split a string into lines, separated by either UNIX, DOS, or legacy
    ///     MacOS newline conventions: `\n`, `\r\n`, or `\r`.
    /// @param[in] string
    ///     Original string
    /// @param[in] keep_empties
    ///     Keep empty substrings from input string.
    ///     This guarantess at least one element in the response.
    /// @param[in] maxsplits
    ///     If non-zero, split at most this number of times
    ///     (returning no more than `maxsplit+1` substrings)
    /// @return
    ///     List of one or more lines

    [[nodiscard]] std::vector<std::string> splitlines(
        const std::string &string,
        uint maxsplits = 0,
        bool keep_empties = false);

    /// Join a sequence of strings by the specified delimiter to an output stream
    /// @param[in] out
    ///     Output stream
    /// @param[in] begin
    ///     Iterator to beginning of sequence
    /// @param[in] end
    ///     Iterator to end of sequence
    /// @param[in] delimiter
    ///     Inserted between each string in vector
    /// @param[in] keep_empties
    ///     Include empty strings from input vector
    /// @param[in] quoted
    ///     Inserted before and after each string in vector

    template <class InputIt>
    void join(
        std::ostream &out,
        const InputIt &begin,
        const InputIt &end,
        const std::string &delimiter = " ",
        bool keep_empties = false,
        bool quoted = false);

    /// @brief
    ///     Join a sequence of strings by the specified delimiter, returning the result
    /// @param[in] begin
    ///     Iterator to beginning of sequence
    /// @param[in] end
    ///     Iterator to end of sequence
    /// @param[in] delimiter
    ///     Inserted between each string in vector
    /// @param[in] keep_empties
    ///     Include empty strings from input vector
    /// @param[in] quoted
    ///     Inserted before and after each string in vector
    /// @return
    ///     New string consisting of each string from \p vector joined by \p delimiter

    template <class InputIt>
    [[nodiscard]] std::string join(
        const InputIt &begin,
        const InputIt &end,
        const std::string &delimiter = " ",
        bool keep_empties = false,
        bool quoted = false);

    /// Join a vector of strings by the specified delimiter
    /// @param[in] vector
    ///     Vector of strings
    /// @param[in] delimiter
    ///     Inserted between each string in vector
    /// @param[in] keep_empties
    ///     Include empty strings from input vector
    /// @param[in] quoted
    ///     Inserted before and after each string in vector
    /// @return
    ///     New string consisting of each string from \p vector joined by \p delimiter
    [[nodiscard]] std::string join(
        const std::vector<std::string> &vector,
        const std::string &delimiter = " ",
        bool keep_empties = false,
        bool quoted = false);

    /// Surround in qutation marks
    [[nodiscard]] std::string quoted(
        const std::string_view &input);

    /// Remove any surrounding quotation marks
    [[nodiscard]] std::string unquoted(
        const std::string_view &input);

    [[nodiscard]] std::string unquoted(
        const std::string &input);

    /// Translate embedded escape sequences
    [[nodiscard]] std::string escaped(
        const std::string_view &input,
        const std::unordered_set<char> &extra_escapes = {});

    void escape(
        std::ostream &out,
        const std::string_view &input,
        const std::unordered_set<char> &extra_escapes = {});

    [[nodiscard]] std::string unescaped(
        const std::string_view &input);

    void unescape(
        std::ostream &out,
        const std::string_view &input);

    /// Escape and quote a string
    [[nodiscard]] std::string to_literal(
        const std::string &input);

    void to_literal(
        std::ostream &stream,
        const std::string_view &input);

    /// Escape and quote a string
    [[nodiscard]] std::string from_literal(
        const std::string &input);

    void from_literal(
        std::ostream &stream,
        const std::string_view &input);

    // Decode %-encoded characters in a URL string
    [[nodiscard]] std::string url_decoded(
        const std::string &encoded_url);

    /// @brief Substitute occurences of a substring with a replacement
    /// @param[in] original
    ///     Text which will be replaced
    /// @param[in] replacement
    ///     Text to take its place
    /// @param[in,out] string
    ///     String in which to perform replacements
    /// @return
    ///     Number of subtitutions performed
    std::size_t substitute(
        const std::string &original,
        const std::string &replacement,
        std::string *string);

    /// @brief Determine whether a string starts with another
    /// @param[in] input
    ///     String to test
    /// @param[in] substring
    ///     Substring to compare against
    /// @param[in] ignore_case
    ///     Perform case insensitive compare
    /// @return
    ///     Indication of whether `input` begins with `substring`.
    [[nodiscard]] bool startswith(
        const std::string &input,
        const std::string &substring,
        bool ignore_case = false);

    /// @brief Determine whether a ends starts with another
    /// @param[in] input
    ///     String to test
    /// @param[in] substring
    ///     Substring to compare against
    /// @param[in] ignore_case
    ///     Perform case insensitive compare
    /// @return
    ///     Indication of whether `input` ends with `substring`.
    [[nodiscard]] bool endswith(
        const std::string &input,
        const std::string &substring,
        bool ignore_case = false);

    [[nodiscard]] bool is_valid_symbol(
        const std::string &input);

    [[nodiscard]] bool is_unicode_text(
        const std::string &input);

    [[nodiscard]] std::string to_unicode_text(
        const std::string &input);

    /// @brief Return the last component of a namespace-qualified string
    /// @param[in] string
    ///     Original name, including path separator
    /// @param[in] separator
    ///     Namespace separator
    /// @return
    ///     Final component of string
    [[nodiscard]] std::string stem(
        const std::string &string,
        const std::string &separator = ":");

    /// @brief Strip specific characters from the beginning and/or end of a string
    /// @param[in] string
    ///     Original name, including path separator
    /// @param[in] lstrip
    ///     Characters to remove from beginning of string
    /// @param[in] rstrip
    ///     Characters to remove from end of string
    /// @return
    ///     Original string with stripped characters removed.
    [[nodiscard]] std::string strip(
        const std::string &string,
        const std::set<char> &lstrip = WHITESPACE,
        const std::set<char> &rstrip = WHITESPACE);

    /// @brief Find the largest common leading substring of several strings
    /// @param[in] strings
    ///     Strings that presumably begin with a common suffix
    /// @return
    ///     Largest common leading substring
    [[nodiscard]] std::string common_prefix(
        const std::vector<std::string> &strings);

    /// @brief concatenate streamable objects into a string
    /// @param[in] args
    ///     Output stream compatible arguments
    /// @return
    ///     String representation of concatenated objects

    template <class... Args>
    [[nodiscard]] std::string to_string(
        const Args &...args) noexcept;

    /// Convert a number to its hexadecimal representation, zero-padded
    /// according to the specifed width or type size.  For instance,
    ///    std::hex((uint16_t)16)
    /// returns the string "0x0010".
    template <typename T>
    [[nodiscard]] std::string hex(
        T val,
        size_t width = sizeof(T) * 2,
        std::string prefix = "0x");

}  // namespace core::str


// Inline definitions
#include "misc.i++"
