/// -*- c++ -*-
//==============================================================================
/// @file qnx-providers.h++
/// @brief Superset of QNX platform providers
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace shared::platform
{
    void register_qnx_providers(const std::string &exec_name);
    void unregister_qnx_providers();
}  // namespace shared::platform
