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

namespace shared::str
{
    void checkstream(const std::istream &is, const std::string &s, const std::type_info &ti)
    {
        if (is.fail() || !is.eof())
        {
            throw std::invalid_argument(
                "Failed to convert string literal to " +
                shared::platform::symbols->cpp_demangle(ti.name(), false) +
                ": \"" + s + "\"");
        }
    }

    bool to_bool(const std::string &s)
    {
        static const std::regex rxfalse("(?:false|no|off)", std::regex_constants::icase);
        static const std::regex rxtrue("(?:true|yes|on)", std::regex_constants::icase);

        // Check negative words
        if (std::regex_match(s, rxfalse))
        {
            return false;
        }
        // Check positive words
        else if (std::regex_match(s, rxtrue))
        {
            return true;
        }

        std::size_t size;

        try
        {
            // Check for int values (incl. base prefix like 0x)
            unsigned long long ull = std::stoull(s, &size, 0);
            if (size > 0)
            {
                return bool(ull);
            }
        }
        catch (const std::exception &)
        {
        }

        // Check for real values
        try
        {
            long double ld = std::stold(s, &size);
            if (size > 0)
            {
                return bool(ld);
            }
        }
        catch (const std::exception &)
        {
        }

        throw std::invalid_argument("not a boolean value: \"" + s + "\"");
    }

    std::string from_bool(bool b)
    {
        return b ? "true" : "false";
    }

}  // namespace shared::str
