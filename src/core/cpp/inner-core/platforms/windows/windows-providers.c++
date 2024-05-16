/// -*- c++ -*-
//==============================================================================
/// @file windows-providers.c++
/// @brief Superset of Windows platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "windows-path.h++"
#include "windows-process.h++"
#include "windows-symbols.h++"
#include "windows-runtime.h++"
#include "windows-timezone.h++"
#include "windows-logsink.h++"

namespace core::platform
{
    void register_windows_providers(const std::string &exec_name)
    {
        symbols.registerProvider<WindowsSymbolsProvider>();
        path.registerProvider<WindowsPathProvider>(exec_name);
        process.registerProvider<WindowsProcessProvider>();
        runtime.registerProvider<WindowsRunTimeProvider>();
        timezone.registerProvider<WindowsTimeZoneProvider>();
        logsink.registerProvider<WindowsLogSinkProvider>(exec_name);
    }

    void unregister_windows_providers()
    {
        logsink.unregisterProvider<WindowsLogSinkProvider>();
        timezone.unregisterProvider<WindowsTimeZoneProvider>();
        runtime.unregisterProvider<WindowsRunTimeProvider>();
        process.unregisterProvider<WindowsProcessProvider>();
        path.unregisterProvider<WindowsPathProvider>();
        symbols.unregisterProvider<WindowsSymbolsProvider>();
    }

    void register_providers(const std::string &exec_name)
    {
        register_windows_providers(exec_name);
    }

    void unregister_providers()
    {
        unregister_windows_providers();
    }
}  // namespace core::platform
