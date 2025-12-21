/// -*- c++ -*-
//==============================================================================
/// @file linux-providers.c++
/// @brief Superset of Linux platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "posix-providers.h++"
#include "linux-path.h++"
#include "linux-symbols.h++"
#include "linux-host.h++"
#include "linux-process.h++"
#ifdef BUILD_DNSSD_AVAHI
#include "linux-dns-sd-avahi.h++"
#endif

namespace core::platform
{
    void register_linux_providers(const std::string &exec_name)
    {
        symbols.registerProvider<LinuxSymbolsProvider>();
        path.registerProvider<LinuxPathProvider>(exec_name);
        host.registerProvider<LinuxHostProvider>();
        process.registerProvider<LinuxProcessProvider>();
#ifdef BUILD_DNSSD_AVAHI
        dns_sd.registerProvider<AvahiServiceDiscoveryProvider>();
#endif
    }

    void unregister_linux_providers()
    {
#ifdef BUILD_DNSSD_AVAHI
        dns_sd.unregisterProvider<AvahiServiceDiscoveryProvider>();
#endif
        process.unregisterProvider<LinuxProcessProvider>();
        host.unregisterProvider<LinuxHostProvider>();
        path.unregisterProvider<LinuxPathProvider>();
        symbols.unregisterProvider<LinuxSymbolsProvider>();
    }

    void register_providers(const std::string &exec_name)
    {
        register_linux_providers(exec_name);
        register_posix_providers(exec_name);
    }

    void unregister_providers()
    {
        unregister_posix_providers();
        unregister_linux_providers();
    }
}  // namespace core::platform
