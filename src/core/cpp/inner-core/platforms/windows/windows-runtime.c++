/// -*- c++ -*-
//==============================================================================
/// @file windows-runtime.c++
/// @brief Run-time environment related functions - WINDOWS systems
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-runtime.h++"

#include <io.h>      // _isatty()
#include <stdlib.h>  // _putenv()

namespace shared::platform
{
    WindowsRunTimeProvider::WindowsRunTimeProvider()
        : Super("WindowsRunTimeProvider")
    {
    }

    bool WindowsRunTimeProvider::isatty(int fd) const
    {
        return _isatty(fd);
    }

    void WindowsRunTimeProvider::putenv(const std::string &envstring)
    {
        _putenv(const_cast<char *>(envstring.data()));
    }
}  // namespace shared::platform
