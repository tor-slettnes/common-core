/// -*- c++ -*-
//==============================================================================
/// @file posix-runtime.c++
/// @brief Run-time environment related functions - POSIX systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-runtime.h++"

#include <stdexcept>

#include <unistd.h>  // isatty()
#include <stdlib.h>  // putenv()

namespace core::platform
{
    PosixRunTimeProvider::PosixRunTimeProvider()
        : Super("PosixRunTimeProvider")
    {
    }

    bool PosixRunTimeProvider::isatty(int fd) const
    {
        return ::isatty(fd);
    }

    void PosixRunTimeProvider::putenv(const std::string &envstring)
    {
        if (int err = ::putenv(const_cast<char *>(envstring.data())))
        {
            throw std::system_error(err, std::system_category());
        }
    }
}  // namespace core::platform
