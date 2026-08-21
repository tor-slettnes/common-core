/// -*- c++ -*-
//==============================================================================
/// @file qnx-providers.h++
/// @brief Superset of QNX platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "types/filesystem.h++"

namespace cc::core::platform
{
    void register_qnx_providers(const fs::path &exec_path);
    void unregister_qnx_providers();
}  // namespace cc::core::platform
