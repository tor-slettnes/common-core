// -*- c++ -*-
//==============================================================================
/// @file upgrade-base.c++
/// @brief Upgrade service - abstract API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-base.h++"

namespace platform::upgrade
{
    core::platform::ProviderProxy<ProviderInterface> upgrade("Upgrade");

}  // namespace platform::upgrade
