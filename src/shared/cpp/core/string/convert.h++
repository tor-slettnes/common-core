/// -*- c++ -*-
//==============================================================================
/// @file convert.h++
/// @brief String conversions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "string/format.h++"

#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <optional>

using namespace std::literals::string_literals;  // ""s

namespace shared::str
{
    // std::string type_description(const std::type_info &ti);
    void checkstream(const std::istream &ss, const std::string &s, const std::type_info &ti);
    bool to_bool(const std::string &s);

    /// Convert string to other arbitrary type T, using istringstream >> operator.
    template <class T>
    struct StringConvert
    {
        static inline T from_string(const std::string &s)
        {
            const static std::regex _rx_hexprefix("0x[[:xdigit:]]+",
                                                  std::regex_constants::icase);
            std::istringstream ss(s);
            T value;

            if (std::regex_match(s, _rx_hexprefix))
                ss >> std::hex;

            ss >> value;
            checkstream(ss, s, typeid(T));
            return value;
        }

        static inline std::string to_string(const T &value)
        {
            std::ostringstream ss;
            ss << value;
            return ss.str();
        }
    };

    /// Partial template specialization for string-to-string conversion
    template <>
    struct StringConvert<std::string>
    {
        static inline std::string from_string(const std::string &s)
        {
            return s;
        }

        static inline std::string to_string(const std::string &s)
        {
            return s;
        }
    };

    /// Partial template specialization for string-to-bool conversion
    template <>
    struct StringConvert<bool>
    {
        static inline bool from_string(const std::string &s)
        {
            return str::to_bool(s);
        }

        static inline std::string to_string(const bool &value)
        {
            std::ostringstream ss;
            ss << std::boolalpha << value;
            return ss.str();
        }
    };

    /// Convert string to the speciefied template type.
    /// \param[in] s
    ///     Input string
    /// \return
    ///     Converted value.
    /// \exception std::exception
    ///     Any exception from underlying conversion routines.

    template <class T>
    inline T convert_to(const std::string &s)
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
    inline T convert_to(const std::string &s,
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
    inline std::string convert_from(const T &value) noexcept
    {
        return StringConvert<T>::to_string(value);
    }
}  // namespace shared::str

namespace std
{
    template <class T1, class T2>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::pair<T1, T2> &pair)
    {
        shared::str::format(stream, "{%r, %r}", pair.first, pair.second);
        return stream;
    }

    template <class T>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::vector<T> &vector)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &element : vector)
        {
            shared::str::format(stream, "%s%r", sep, element);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class K, class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::map<K, V> &map)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &kv : map)
        {
            shared::str::format(stream, "%s%r: %r", sep, kv.first, kv.second);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class K, class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::unordered_map<K, V> &map)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &kv : map)
        {
            shared::str::format(stream, "%s%r: %r", sep, kv.first, kv.second);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::set<V> &set)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &v : set)
        {
            shared::str::format(stream, "%s%r", sep, v);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class V, class C>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::set<V, C> &set)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &v : set)
        {
            shared::str::format(stream, "%s%r", sep, v);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::unordered_set<V> &set)
    {
        stream << "{";
        std::string sep = "";
        for (const auto &v : set)
        {
            shared::str::format(stream, "%s%r", sep, v);
            sep = ", ";
        }
        stream << "}";
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::optional<V> &opt)
    {
        if (opt.has_value())
        {
            shared::str::format(stream, "%r", opt.value());
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::shared_ptr<V> &ptr)
    {
        if (ptr)
        {
            shared::str::format(stream, "{%r}", *ptr);
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::unique_ptr<V> &ptr)
    {
        if (ptr)
        {
            shared::str::format(stream, "{%r}", *ptr);
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }

    template <class V>
    inline std::ostream &operator<<(std::ostream &stream,
                                    const std::weak_ptr<V> &weakptr)
    {
        if (auto sptr = weakptr.lock())
        {
            shared::str::format(stream, "{%r}", *sptr);
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }

}  // namespace std
