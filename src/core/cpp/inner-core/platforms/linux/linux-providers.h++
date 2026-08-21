/// -*- c++ -*-
//==============================================================================
/// @file linux-providers.h++
/// @brief Superset of Linux platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/filesystem.h++"
#include <string>

namespace cc::core::platform
{
    void register_linux_providers(const fs::path &exec_path);
    void unregister_linux_providers();
}  // namespace cc::core::platform
