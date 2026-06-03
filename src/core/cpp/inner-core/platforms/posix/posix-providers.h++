/// -*- c++ -*-
//==============================================================================
/// @file posix-providers.h++
/// @brief Superset of POSIX platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include <string>

namespace cc::core::platform
{
    void register_posix_providers(const std::string &exec_name);
    void unregister_posix_providers();
}  // namespace cc::core::platform
