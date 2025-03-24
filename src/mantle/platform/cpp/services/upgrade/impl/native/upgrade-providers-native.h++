// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-native.h++
/// @brief UPGRADE service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace upgrade::native
{
    void register_providers();
    void unregister_providers();
}  // namespace upgrade::native
