/// -*- c++ -*-
//==============================================================================
/// @file providers.h++
/// @brief Superset of platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string>

namespace cc::core::platform
{
    extern void register_providers(const std::string &exec_name);
    extern void unregister_providers();
}  // namespace cc::core::platform
