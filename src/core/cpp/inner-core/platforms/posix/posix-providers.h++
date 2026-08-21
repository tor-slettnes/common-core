/// -*- c++ -*-
//==============================================================================
/// @file posix-providers.h++
/// @brief Superset of POSIX platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/filesystem.h++"
#include <string>

namespace cc::core::platform
{
    void register_posix_providers(const fs::path &exec_path);
    void unregister_posix_providers();
}  // namespace cc::core::platform
