// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-native.h++
/// @brief UPGRADE service - Native implementation wrapper
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once

namespace platform::upgrade::native
{
    void register_providers();
    void unregister_providers();
}  // namespace platform::upgrade::native
