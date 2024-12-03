/// -*- c++ -*-
//==============================================================================
/// @file cutils.c++
/// @brief Misc. utility functions for interacting with C/system calls
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include <string>
#include <type_traits>
#include <system_error>

namespace core::io
{
    template <class T, std::enable_if_t<std::is_integral_v<T>, bool> = true>
    T checkstatus(T status, const std::string &context = {})
    {
        if (status < 0)
        {
            throw std::system_error(errno, std::system_category(), context);
        }
        return status;
    }

    template <class PT>
    PT *checkstatus(PT *pointer, const std::string &context = {})
    {
        if (pointer == nullptr)
        {
            throw std::system_error(errno, std::system_category(), context);
        }
        return pointer;
    }

} // namespace core::io
