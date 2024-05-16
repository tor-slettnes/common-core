/// -*- c++ -*-
//==============================================================================
/// @file posix-symbols.c++
/// @brief Internal symbols - POSIX specifics
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-symbols.h++"
#include "string/misc.h++"

#include <cxxabi.h>

namespace core::platform
{
    PosixSymbolsProvider::PosixSymbolsProvider(const std::string &name)
        : Super(name)
    {
    }

    std::string PosixSymbolsProvider::cpp_demangle(const std::string &abiname, bool stem_only) const noexcept
    {
        int status;
        if (char *ret = abi::__cxa_demangle(abiname.data(), nullptr, 0, &status))
        {
            std::string demangled(ret);
            free(ret);

            return stem_only ? str::stem(demangled, ":") : demangled;
        }
        else
        {
            return "";
        }
    };

}  // namespace core::platform
