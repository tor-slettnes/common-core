/// -*- c++ -*-
//==============================================================================
/// @file dns-sd.c++
/// @brief DNS service discovery interface
/// @author Tor Slettnes
//==============================================================================

#include "dns-sd.h++"

namespace core::platform
{
    ProviderProxy<ServiceDiscoveryProvider> dns_sd("DNS Service Discovery");
}  // namespace core::platform
