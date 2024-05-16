/// -*- c++ -*-
//==============================================================================
/// @file windows-symbols.c++
/// @brief Internal symbols - Windows specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-symbols.h++"
#include "string/misc.h++"

#include <windows.h>
#include <dbghelp.h>

// DWORD IMAGEAPI UnDecorateSymbolName(
//   [in]  PCSTR name,
//   [out] PSTR  outputString,
//   [in]  DWORD maxStringLength,
//   [in]  DWORD flags
// );

namespace cc::platform
{
    WindowsSymbolsProvider::WindowsSymbolsProvider()
        : Super("WindowsSymbolsProvider")
    {
    }

    std::string WindowsSymbolsProvider::cpp_demangle(const std::string &abiname, bool stem_only) const noexcept
    {
        char demangled[1024];
        if (UnDecorateSymbolName(abiname.c_str(), demangled, sizeof(demangled), 0x0000))
        {
            return stem_only ? str::stem(demangled, ":") : std::string(demangled);
        }
        else
        {
            return "";
        }
    };

}  // namespace cc::platform
