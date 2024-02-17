/// -*- c++ -*-
//==============================================================================
/// @file misc.h++
/// @brief Miscellaneous string manipulations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/format.h++"

#include <iomanip>
#include <iostream>
#include <locale>
#include <regex>
#include <sstream>
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

/// Miscellaneous string manipulations
namespace shared::str
{
    //    constexpr auto WHITESPACE = { ' ', '\t', '\r', '\n', 'f', '\0' };

#if defined(_WIN32) || defined(_WIN64)
    constexpr auto NEWLINE = "\r\n";
#else
    constexpr auto NEWLINE = "\n";
#endif

    /// Convert string to uppercase in place, using the specified locale or current locale.
    /// \param[in,out] s String to convert.
    /// \param[in] loc Locale. Defaults to global locale.
    void toupper(
        std::string *s,
        const std::locale &loc = std::locale());

    /// Convert string to lowercase, using the specified locale or current locale.
    /// \param[in] s
    ///     Original string
    /// \param[in] loc
    ///     Locale. Defaults to global locale.
    void tolower(
        std::string *s,
        const std::locale &loc = std::locale());

    /// Convert string to uppercase, using the specified locale or current locale.
    /// \param[in] s
    ///     Original string
    /// \param[in] loc
    ///     Locale. Defaults to global locale.
    /// \return Copy of string converted to uppercsae.
    [[nodiscard]] std::string toupper(
        const std::string &s,
        const std::locale &loc = std::locale());

    /// \brief Convert string to lowercase in place, using the specified locale or current locale.
    /// \param[in,out] s
    ///      String to convert.
    /// \param[in] loc
    ///      Locale. Defaults to global locale.
    /// \return Copy of string converted to lowercase.
    [[nodiscard]] std::string tolower(
        const std::string &s,
        const std::locale &loc = std::locale());

    /// \brief Convert from (UTF8-encoded) `std::string` to (UTF16 or UCS2-encoded) `std::wstring`)
    /// \param[in] wstr
    ///      Wide string
    /// \return
    ///      Byte string
    [[nodiscard]] std::string from_wstring(
        const std::wstring &wstr);

    [[nodiscard]] std::string from_wstring(
        const wchar_t *begin,
        std::size_t size);

    /// \brief Convert from `std::wstring` to `std::string`
    /// \param[in] str
    ///      ByteWide string
    /// \return
    ///      Wide string
    [[nodiscard]] std::wstring to_wstring(
        const std::string &str);

    [[nodiscard]] std::wstring to_wstring(
        const char *begin,
        std::size_t size);

    /// \brief Return representation of a string with characters replaced by asterisks
    /// \param[in] string
    ///     Input string
    /// \return
    ///     String with characters replaced by asterisks
    [[nodiscard]] std::string obfuscated(
        const std::string &s);

    /// Return a universally unique identifier (UUID)
    // std::string uuid();

    /// Wrap words in a string to fit between left and right margins.
    /// \param[in] input
    ///     Input string
    /// \param[in] start_column
    ///     Current output column, subtraced from the left margin of the first line
    /// \param[in] left_margin
    ///     Number of whitespaces to insert at the beginning of each new line.
    /// \param[in] right_margin
    ///     Wrap column. Output lines MAY exceed this length if a single word cannot
    ///     otherwise fit on one line.
    /// \param[in] keep_empties
    ///     Include empty strings in case of successive spaces in input string
    /// \return
    ///     Output string.

    [[nodiscard]] std::string wrap(
        const std::string &input,
        size_t start_column = 0,
        size_t left_margin = 24,
        size_t right_margin = 80,
        bool keep_empties = false);

    /// \fn wrap
    /// \brief Wrap words to fit between left and right margins.
    /// \param[in] words
    ///     Vector of words to wrap
    /// \param[in] start_column
    ///     Current output column, subtraced from the left margin of the first line
    /// \param[in] left_margin
    ///     Number of whitespaces to insert at the beginning of each new line.
    /// \param[in] right_margin
    ///     Wrap column. Output lines MAY exceed this length if a single word cannot
    ///     otherwise fit on one line.
    /// \return
    ///     Output string.

    [[nodiscard]] std::string wrap(
        const std::vector<std::string> &words,
        size_t start_column = 0,
        size_t left_margin = 24,
        size_t right_margin = 80);

    /// \brief
    ///     Obtain first nonempty portion of an input string, split by a token
    /// \param[in] string
    ///     Input string
    /// \param[in] delimiter
    ///     Separator by which to split the input string
    /// \param[out] first
    ///     The first nonempty part of the input string after splitting if any;
    ///     otherwise not modified.
    /// \param[out] more
    ///     Optional pointer to a boolean that will receive indication of
    ///     whether the input string contains additional parts
    /// \return
    ///     Indicates whether the string contained at least one nonempty part.
    bool getfirst(
        const std::string &string,
        const std::string &delimiter,
        std::string *first,
        bool *more = nullptr);

    /// \fn split
    /// \brief
    ///     Separate a string at each occurence of \p delimiter into one or more
    ///     substrings.
    /// \param[in] string
    ///     Original string
    /// \param[in] delimiter
    ///     Separator by which to split the input string
    /// \param[in] keep_empties
    ///     Keep empty substrings from input string.
    ///     This guarantess at least one element in the response.
    /// \param[in] maxsplits
    ///     If non-zero, split at most this number of times
    ///     (returning no more than `maxsplit+1` substrings)
    /// \return
    ///     Vector of one or more non-overlapping substrings of \p string
    ///     separated by \p delimiter

    [[nodiscard]] std::vector<std::string> split(
        const std::string &string,
        const std::string &delimiter = " ",
        uint maxsplits = 0,
        bool keep_empties = false);

    /// Join a vector of strings by the specified delimiter
    /// \param[in] vector
    ///     Vector of strings
    /// \param[in] delimiter
    ///     Inserted between each string in vector
    /// \param[in] keep_empties
    ///     Include empty strings from input vector
    /// \param[in] quoted
    ///     Inserted before and after each string in vector
    /// \return
    ///     New string consisting of each string from \p vector joined by \p delimiter
    [[nodiscard]] std::string join(
        const std::vector<std::string> &vector,
        const std::string &delimiter = " ",
        bool keep_empties = false,
        bool quoted = false);

    /// Surround in qutation marks
    [[nodiscard]] std::string quoted(
        const std::string &input);

    /// Remove any surrounding quotation marks
    [[nodiscard]] std::string unquoted(
        const std::string &input);

    /// Translate embedded escape sequences
    [[nodiscard]] std::string unescaped(
        const std::string &input);

    /// \brief Substitute occurences of a substring with a replacement
    /// \param[in] original
    ///     Text which will be replaced
    /// \param[in] replacement
    ///     Text to take its place
    /// \param[in,out] string
    ///     String in which to perform replacements
    void substitute(
        const std::string &original,
        const std::string &replacement,
        std::string *string);

    /// \brief Determine whether a string starts with another
    /// \param[in] input
    ///     String to test
    /// \param[in] substring
    ///     Substring to compare against
    /// \return
    ///     Indication of whether `input` begins with `substring`.
    [[nodiscard]] bool startswith(
        const std::string &input,
        const std::string &substring);

    /// \brief Determine whether a ends starts with another
    /// \param[in] input
    ///     String to test
    /// \param[in] substring
    ///     Substring to compare against
    /// \return
    ///     Indication of whether `input` ends with `substring`.
    [[nodiscard]] bool endswith(
        const std::string &input,
        const std::string &substring);

    /// \brief Return the last component of a namespace-qualified string
    /// \param[in] string
    ///     Original name, including path separator
    /// \param[in] separator
    ///     Namespace separator
    /// \return
    ///     Final component of string
    [[nodiscard]] std::string stem(
        const std::string &string,
        const std::string &separator = ":");

    /// \brief Strip specific characters from the beginning and/or end of a string
    /// \param[in] string
    ///     Original name, including path separator
    /// \param[in] lstrip
    ///     Characters to remove from beginning of string
    /// \param[in] rstrip
    ///     Characters to remove from end of string
    /// \return
    ///     Original string with stripped characters removed.
    [[nodiscard]] std::string strip(
        const std::string &string,
        const std::set<char> &lstrip = WHITESPACE,
        const std::set<char> &rstrip = WHITESPACE);

    /// \brief concatenate streamable objects into a string
    /// \param[in] args
    ///     Output stream compatible arguments
    /// \return
    ///     String representation of concatenated objects

    template <class... Args>
    [[nodiscard]] std::string to_string(
        const Args &...args) noexcept
    {
        std::stringstream ss;
        (ss << ... << args);
        return ss.str();
    }

    /// Convert a number to its hexadecimal representation, zero-padded
    /// according to the specifed width or type size.  For instance,
    ///    std::hex((uint16_t)16)
    /// returns the string "0x0010".
    template <typename T>
    [[nodiscard]] std::string hex(
        T val,
        size_t width = sizeof(T) * 2,
        std::string prefix = "0x")
    {
        std::stringstream os;
        os << prefix << std::setfill('0') << std::setw(width) << std::hex << (val | 0);
        return os.str();
    }

    //std::string mangle_name (const std::string &name, const std::regex &tokens="(\\W)");

}  // namespace shared::str
