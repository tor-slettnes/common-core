/// -*- c++ -*-
//==============================================================================
/// @file providers.h++
/// @brief Superset of platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <string>

namespace cc::platform
{
    extern void register_providers(const std::string &exec_name);
    extern void unregister_providers();
}  // namespace cc::platform
