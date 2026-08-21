/// -*- c++ -*-
//==============================================================================
/// @file windows-providers.c++
/// @brief Superset of Windows platform providers
/// @author Tor Slettnes
//==============================================================================

#include "windows-path.h++"
#include "windows-process.h++"
#include "windows-symbols.h++"
#include "windows-runtime.h++"
#include "windows-timezone.h++"
#include "windows-logsink.h++"

namespace cc::core::platform
{
    void register_windows_providers(const fs::path &exec_path)
    {
        symbols.registerProvider<WindowsSymbolsProvider>();
        path.registerProvider<WindowsPathProvider>(exec_path);
        process.registerProvider<WindowsProcessProvider>();
        runtime.registerProvider<WindowsRunTimeProvider>();
        timezone.registerProvider<WindowsTimeZoneProvider>();
        logsink.registerProvider<WindowsLogSinkProvider>(exec_path);
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

    void register_providers(const fs::path &exec_path)
    {
        register_windows_providers(exec_path);
    }

    void unregister_providers()
    {
        unregister_windows_providers();
    }
}  // namespace cc::core::platform
