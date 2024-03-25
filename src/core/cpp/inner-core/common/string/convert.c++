/// -*- c++ -*-
//==============================================================================
/// @file convert.c++
/// @brief String conversions
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "string/convert.h++"
#include "string/misc.h++"
#include "platform/symbols.h++"

#include <typeinfo>
#include <unordered_map>
#include <string>

namespace core::str
{
    void checkstream(const std::istream &is,
                     const std::string_view &s,
                     const std::type_info &ti)
    {
        if (is.fail() || !is.eof())
        {
            throw std::invalid_argument(
                str::format("Failed to convert string literal to %s: %r",
                            core::platform::symbols->cpp_demangle(ti.name(), false),
                            s));
        }
    }

    //==========================================================================
    // String-to-string passthrough

    std::string StringConvert<std::string>::from_string(const std::string_view &s)
    {
        return {s.begin(), s.end()};
    }

    std::string StringConvert<std::string>::to_string(const std::string &s)
    {
        return s;
    }

    //==========================================================================
    // Boolean integer conversions

    bool StringConvert<bool>::from_string(const std::string_view &s)
    {
        static const std::regex rxfalse("(?:false|no|off)", std::regex_constants::icase);
        static const std::regex rxtrue("(?:true|yes|on)", std::regex_constants::icase);

        // Check negative words
        if (std::regex_match(s.begin(), s.end(), rxfalse))
        {
            return false;
        }
        // Check positive words
        else if (std::regex_match(s.begin(), s.end(), rxtrue))
        {
            return true;
        }

        try
        {
            return convert_to<std::int64_t>(s) != 0;
        }
        catch (const std::exception &)
        {
        }

        try
        {
            return convert_to<double>(s) != 0.0;
        }
        catch (const std::exception &)
        {
        }

        throw std::invalid_argument(
            str::format("not a boolean value: %r", s));
    }

    std::string StringConvert<bool>::to_string(const bool &value)
    {
        return value ? "true" : "false";
    }

}  // namespace core::str
