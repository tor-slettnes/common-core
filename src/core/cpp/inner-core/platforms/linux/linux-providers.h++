/// -*- c++ -*-
//==============================================================================
/// @file linux-providers.h++
/// @brief Superset of Linux platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string>

namespace cc::core::platform
{
    void register_linux_providers(const std::string& exec_name);
    void unregister_linux_providers();
}  // namespace cc::core::platform
