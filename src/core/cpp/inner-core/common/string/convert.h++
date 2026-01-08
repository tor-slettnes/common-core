/// -*- c++ -*-
//==============================================================================
/// @file convert.h++
/// @brief String conversions
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <typeinfo>
#include <charconv>
#include <string>
#include <string_view>
#include <sstream>
#include <optional>

#include <iostream>  // temp
#include <iomanip>   // temp

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
    class StringConvert
    {
    public:
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

    template <class T>
    class StringConvert<T, typename std::enable_if_t<std::is_integral_v<T>>>
    {
    public:
        static T from_string(const std::string_view &s)
        {
            static const std::errc ok{};
            const char *start = &*s.begin();
            const char *end = &*s.end();
            int base = 10;

            if ((start != end) && (*start == '+'))
            {
                start++;
            }

            if ((std::distance(start, end) > 2) &&
                (*start == '0') &&
                (std::tolower(*(start + 1)) == 'x'))
            {
                start += 2;
                base = 16;
            }

            T value = 0;
            auto [ptr, ec] = std::from_chars(start, end, value, base);

            if ((ec == ok) && (ptr == end))
            {
                return value;
            }
            else if (ec != ok)
            {
                throw std::invalid_argument(std::make_error_code(ec).message());
            }
            else
            {
                throw std::invalid_argument("Not all characters converted");
            }
        }

        static std::string to_string(const T &value)
        {
            std::ostringstream ss;
            ss << value;
            return ss.str();
        }
    };

    //==========================================================================
    /// Floating pointconversions

    template <class T>
    class StringConvert<T, typename std::enable_if_t<std::is_floating_point_v<T>>>
    {
    public:
        static T from_string(const std::string_view &s)
        {
            static const std::errc ok{};
            const char *start = &*s.begin();
            const char *end = &*s.end();

            if ((start != end) && (*start == '+'))
            {
                // std::from_chars does not accept leading `+`, so skip it.
                start++;
            }

            T value = 0.0;
            auto [ptr, ec] = std::from_chars(start, end, value);
            if (ec != ok)
            {
                throw std::invalid_argument(std::make_error_code(ec).message());
            }
            else if (ptr != end)
            {
                throw std::invalid_argument("Not all characters converted");
            }
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
    /// Partial template specialization for string<>string passthrough
    template <>
    class StringConvert<std::string>
    {
    public:
        static std::string from_string(const std::string_view &s);
        static std::string to_string(const std::string &s);
    };

    //==========================================================================
    // Partial template specialization for string<>bool conversion
    template <>
    class StringConvert<bool>
    {
    public:
        static bool from_string(const std::string_view &s);
        static std::string to_string(const bool &value);
    };

    //==========================================================================
    /// Convert string to the speciefied template type.
    /// @param[in] s
    ///     Input string
    /// @return
    ///     Converted value.
    /// @exception std::exception
    ///     Any exception from underlying conversion routines.

    template <class T>
    T convert_to(const std::string_view &s)
    {
        return StringConvert<T>::from_string(s);
    }

    /// @brief Convert string to the specified template type.
    /// @param[in] s
    ///     Input string
    /// @param[in] fallback
    ///     Value to return if conversion cannot take place
    /// @param[out] eptr
    ///     Any exception that occurred ducring conversion. Pass in a `nullptr`
    ///     to completely disable exception handling.
    /// @return
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

    /// @brief Convert string to the specified template type.
    /// @param[in] s
    ///     Optional input string. If not provided, fallback value is returned.
    /// @param[in] fallback
    ///     Value to return if conversion cannot take place
    /// @param[out] eptr
    ///     Any exception that occurred ducring conversion. Pass in a `nullptr`
    ///     to completely disable exception handling.
    /// @return
    ///     Converted value.
    ///
    /// This variant does not throw exceptions. Instead, any exceptions thrown
    /// by underlying conversion methods are stored in the `eptr` argument, can
    /// can be accessed via `std::rethrow_exception()`.

    template <class T>
    T convert_optional_to(const std::optional<std::string_view> &s,
                          const T &fallback,
                          std::exception_ptr *eptr = nullptr) noexcept
    {
        if (s)
        {
            try
            {
                return convert_to<T>(*s);
            }
            catch (...)
            {
                if (eptr)
                {
                    *eptr = std::current_exception();
                }
            }
        }

        return fallback;
    }

    /// @brief Try converting string to the specified template type.
    /// @param[in] s
    ///     Input string
    /// @return
    ///     Optional container containing converted value iff successful.

    template <class T>
    std::optional<T> try_convert_to(const std::string_view &s)
    {
        try
        {
            return convert_to<T>(s);
        }
        catch (...)
        {
            return {};
        }
    }

    /// Convert an arbitrary type to string
    /// @param[in] value
    ///     Value to be converted to string
    /// @return
    ///     String representation of value.
    template <class T>
    std::string convert_from(const T &value) noexcept
    {
        return StringConvert<T>::to_string(value);
    }

    template <class T>
    std::string convert_from(const std::optional<T> &value) noexcept
    {
        if (value)
        {
            return StringConvert<T>::to_string(value.value());
        }
        else
        {
            return {};
        }
    }

}  // namespace core::str
