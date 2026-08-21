/// -*- c++ -*-
//==============================================================================
/// @file windows-providers.h++
/// @brief Superset of Windows platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/filesystem.h++"

namespace cc::core::platform
{
    void register_windows_providers(const fs::path &exec_path);
    void unregister_windows_providers();
}  // namespace cc::core::platform
