/// -*- c++ -*-
//==============================================================================
/// @file linux-symbols.c++
/// @brief Functions to produce symbols - Linux verison
/// @author Tor Slettnes
//==============================================================================

#include "linux-symbols.h++"
#include "errno-list.gen"

#include <uuid.h>

namespace core::platform
{
    LinuxSymbolsProvider::LinuxSymbolsProvider(const std::string &name)
        : PosixSymbolsProvider(name)
    {
    }

    std::string LinuxSymbolsProvider::uuid() const noexcept
    {
        uuid_t uuid;
        uuid_generate(uuid);

        char buffer[UUID_STR_LEN];
        uuid_unparse(uuid, buffer);
        return buffer;
    }

    std::string LinuxSymbolsProvider::errno_name(int num) const noexcept
    {
        if (auto it = errno_names.find(num); it != errno_names.end())
        {
            return it->second;
        }
        else
        {
            return "unknown";
        }
    }

}  // namespace core::platform
