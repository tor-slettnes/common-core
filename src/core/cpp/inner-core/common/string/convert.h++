/// -*- c++ -*-
//==============================================================================
/// @file convert.h++
/// @brief String conversions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "format.h++"
#include "stream.h++"
#include <charconv>

using namespace std::literals::string_literals;  // ""s

namespace core::str
{
    // std::string type_description(const std::type_info &ti);
    void checkstream(const std::istream &ss,
                     const std::string_view &s,
                     const std::type_info &ti);

    //==========================================================================
    /// Convert string to other arbitrary type T, using istringstream >> operator.

    template <class T, class Enable = void>
    struct StringConvert
    {
        static T from_string(const std::string_view &s)
        {
            T value;
            std::stringstream ss;
            ss << s;
            ss >> value;
            checkstream(ss, s, typeid(T));
            return value;
        }

        static std::string to_string(const T &value)
        {
            std::ostringstream ss;
            ss << value;
            return ss.str();
        }
    };

    //==========================================================================
    /// Integral type conversions

    // template <class T>
    // struct StringConvert<T,
    //                      typename std::enable_if_t<std::is_integral_v<T>>>
    // {
    //     static T from_string(const std::string_view &s)
    //     {
    //         static const std::errc ok{};
    //         const char *start = &*s.begin();
    //         const char *end = &*s.end();

    //         T value = 0;
    //         // First we try to parse the string as a decimal number
    //         std::cout << "Converting as integral" << std::endl;
    //         auto [ptr, ec] = std::from_chars(start, end, value);
    //         if ((ec == ok) && (ptr == end))
    //         {
    //             return value;
    //         }
    //         else
    //         {
    //             // Next, we try to parse it without an explicit base, allowing
    //             // `0x` to indicate hexadecimal. (The reason we didn't do this
    //             // in the first place is that a leading `0` would then be
    //             // considered an octal indicator).
    //             std::cout << "Converting as integral with base 0" << std::endl;
    //             auto [ptr2, ec2] = std::from_chars(start, end, value, 0);
    //             if ((ec2 == ok) && (ptr2 == end))
    //             {
    //                 return value;
    //             }
    //             else if (ec != ok)
    //             {
    //                 throw std::invalid_argument(std::make_error_code(ec).message());
    //             }
    //             else
    //             {
    //                 throw std::invalid_argument(
    //                     str::format("Not all characters converted: %s", s));
    //             }
    //         }
    //     }

    //     static std::string to_string(const T &value)
    //     {
    //         std::ostringstream ss;
    //         ss << value;
    //         return ss.str();
    //     }
    // };

    //==========================================================================
    /// Floating pointconversions

    // template <class T>
    // struct StringConvert<T,
    //                      typename std::enable_if_t<std::is_floating_point_v<T>>>
    // {
    //     static T from_string(const std::string_view &s)
    //     {
    //         static const std::errc ok{};
    //         const char *start = &*s.begin();
    //         const char *end = &*s.end();

    //         T value = 0.0;
    //         auto [ptr, ec] = std::from_chars(start, end, value);
    //         if (ec != ok)
    //         {
    //             throw std::invalid_argument(std::make_error_code(ec).message());
    //         }
    //         else if (ptr != end)
    //         {
    //             throw std::invalid_argument(
    //                 str::format("Not all characters converted: %s", s));
    //         }
    //         return value;
    //     }

    //     static std::string to_string(const T &value)
    //     {
    //         std::ostringstream ss;
    //         ss << value;
    //         return ss.str();
    //     }
    // };

    //==========================================================================
    /// Partial template specialization for string<>string passthrough
    template <>
    struct StringConvert<std::string>
    {
        static std::string from_string(const std::string_view &s);
        static std::string to_string(const std::string &s);
    };

    //==========================================================================
    // Partial template specialization for string<>bool conversion
    template <>
    struct StringConvert<bool>
    {
        static bool from_string(const std::string_view &s);
        static std::string to_string(const bool &value);
    };

    //==========================================================================
    /// Convert string to the speciefied template type.
    /// \param[in] s
    ///     Input string
    /// \return
    ///     Converted value.
    /// \exception std::exception
    ///     Any exception from underlying conversion routines.

    template <class T>
    T convert_to(const std::string_view &s)
    {
        return StringConvert<T>::from_string(s);
    }

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
    T convert_to(const std::string_view &s,
                 const T &fallback,
                 std::exception_ptr *eptr = nullptr) noexcept
    {
        try
        {
            return convert_to<T>(s);
        }
        catch (...)
        {
            if (eptr)
            {
                *eptr = std::current_exception();
            }
            return fallback;
        }
    }

    /// Convert an arbitrary type to string
    /// \param[in] value
    ///     Value to be converted to string
    /// \return
    ///     String representation of value.
    template <class T>
    std::string convert_from(const T &value) noexcept
    {
        return StringConvert<T>::to_string(value);
    }
}  // namespace core::str
