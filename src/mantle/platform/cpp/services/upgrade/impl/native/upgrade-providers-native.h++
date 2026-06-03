// -*- c++ -*-
//==============================================================================
/// @file upgrade-providers-native.h++
/// @brief UPGRADE service - Native implementation wrapper
/// @author Tor Slettnes
//==============================================================================

#pragma once

namespace cc::platform::upgrade::native
{
    void register_providers();
    void unregister_providers();
}  // namespace cc::platform::upgrade::native
