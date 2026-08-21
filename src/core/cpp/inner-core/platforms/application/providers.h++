/// -*- c++ -*-
//==============================================================================
/// @file providers.h++
/// @brief Superset of platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/filesystem.h++"
#include <string>

namespace cc::core::platform
{
    extern void register_providers(const fs::path &exec_path);
    extern void unregister_providers();
}  // namespace cc::core::platform
