/// -*- c++ -*-
//==============================================================================
/// @file linux-providers.h++
/// @brief Superset of Linux platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <string>

namespace cc::platform
{
    void register_linux_providers(const std::string &exec_name);
    void unregister_linux_providers();
}  // namespace cc::platform
