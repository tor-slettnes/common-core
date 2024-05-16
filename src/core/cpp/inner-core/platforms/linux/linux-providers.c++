/// -*- c++ -*-
//==============================================================================
/// @file linux-providers.c++
/// @brief Superset of Linux platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-providers.h++"
#include "linux-path.h++"
#include "linux-symbols.h++"

namespace core::platform
{
    void register_linux_providers(const std::string &exec_name)
    {
        symbols.registerProvider<LinuxSymbolsProvider>();
        path.registerProvider<LinuxPathProvider>(exec_name);
    }

    void unregister_linux_providers()
    {
        path.unregisterProvider<LinuxPathProvider>();
        symbols.unregisterProvider<LinuxSymbolsProvider>();
    }

    void register_providers(const std::string &exec_name)
    {
        register_posix_providers(exec_name);
        register_linux_providers(exec_name);
    }

    void unregister_providers()
    {
        unregister_linux_providers();
        unregister_posix_providers();
    }
}  // namespace core::platform
