/// -*- c++ -*-
//==============================================================================
/// @file windows-providers.h++
/// @brief Superset of Windows platform providers
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace core::platform
{
    void register_windows_providers(const std::string &exec_name);
    void unregister_windows_providers();
}  // namespace core::platform
