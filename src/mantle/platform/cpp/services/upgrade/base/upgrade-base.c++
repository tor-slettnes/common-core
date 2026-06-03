// -*- c++ -*-
//==============================================================================
/// @file upgrade-base.c++
/// @brief Upgrade service - abstract API
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-base.h++"

namespace cc::platform::upgrade
{
    core::platform::ProviderProxy<ProviderInterface> upgrade("Upgrade");

}  // namespace cc::platform::upgrade
