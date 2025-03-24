// -*- c++ -*-
//==============================================================================
/// @file upgrade-base.c++
/// @brief Upgrade service - abstract API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-base.h++"

namespace upgrade
{
    core::platform::ProviderProxy<ProviderInterface> upgrade("Upgrade");

}  // namespace upgrade
