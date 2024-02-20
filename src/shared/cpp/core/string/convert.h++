/// -*- c++ -*-
//==============================================================================
/// @file convert.h++
/// @brief String conversions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "format.h++"
#include "stream.h++"

using namespace std::literals::string_literals;  // ""s

namespace shared::str
{
    // std::string type_description(const std::type_info &ti);
    void checkstream(const std::istream &ss, const std::string &s, const std::type_info &ti);
    bool to_bool(const std::string &s);
    std::string from_bool(bool b);

    /// Convert string to other arbitrary type T, using istringstream >> operator.
    template <class T>
    struct StringConvert
    {
        static T from_string(const std::string &s);
        static std::string to_string(const T &value);
    };

    /// Partial template specialization for string<>string conversion
    template <>
    struct StringConvert<std::string>
    {
        static std::string from_string(const std::string &s);
        static std::string to_string(const std::string &s);
    };

    /// Partial template specialization for string<>bool conversion
    template <>
    struct StringConvert<bool>
    {
        static bool from_string(const std::string &s);
        static std::string to_string(const bool &value);
    };

    /// Convert string to the speciefied template type.
    /// \param[in] s
    ///     Input string
    /// \return
    ///     Converted value.
    /// \exception std::exception
    ///     Any exception from underlying conversion routines.

    template <class T>
    T convert_to(const std::string &s);

    /// \brief Convert string to the specified template type.
    /// \param[in] s
    ///     Input string
    /// \param[in] fallback
    ///     Value to return if conversion cannot take place
    /// \param[out] eptr
    ///     Any exception that occurred ducring conversion. Pass in a `nullptr`
    ///     to completely disable exception handling.
    /// \return
    ///     Converted value.
    ///
    /// This variant does not throw exceptions. Instead, any exceptions thrown
    /// by underlying conversion methods are stored in the `eptr` argument, can
    /// can be accessed via `std::rethrow_exception()`.

    template <class T>
    T convert_to(const std::string &s,
                 const T &fallback,
                 std::exception_ptr *eptr = nullptr) noexcept;

    /// Convert an arbitrary type to string
    /// \param[in] value
    ///     Value to be converted to string
    /// \return
    ///     String representation of value.
    template <class T>
    std::string convert_from(const T &value) noexcept;
}  // namespace shared::str

#include "convert.i++"
