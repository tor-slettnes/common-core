/// -*- c++ -*-
//==============================================================================
/// @file windows-providers.h++
/// @brief Superset of Windows platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace shared::platform
{
    void register_windows_providers(const std::string &exec_name);
    void unregister_windows_providers();
}  // namespace shared::platform
