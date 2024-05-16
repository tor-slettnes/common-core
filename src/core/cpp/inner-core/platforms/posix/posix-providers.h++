/// -*- c++ -*-
//==============================================================================
/// @file posix-providers.h++
/// @brief Superset of POSIX platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include <string>

namespace cc::platform
{
    void register_posix_providers(const std::string &exec_name);
    void unregister_posix_providers();
}  // namespace cc::platform
