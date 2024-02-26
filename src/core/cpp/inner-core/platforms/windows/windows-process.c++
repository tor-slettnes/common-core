// -*- c++ -*-
//==============================================================================
/// @file windows-process.c++
/// @brief Process invocation - WINDOWS implementations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-process.h++"
#include <windows.h>
#include <process.h>  // _getpid()

namespace core::platform
{
    WindowsProcessProvider::WindowsProcessProvider()
        : Super("WindowsProcessProvider")
    {
    }

    PID WindowsProcessProvider::thread_id() const
    {
        return GetCurrentThreadId();
    }

    PID WindowsProcessProvider::process_id() const
    {
        return _getpid();
    }

}  // namespace core::platform
