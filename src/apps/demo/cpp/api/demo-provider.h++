// -*- c++ -*-
//==============================================================================
/// @file demo-provider.h++
/// @brief Example App - interface
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"      // To invoke functionalty directly
#include "demo-signals.h++"  // To emit/receive demo data updates

#include <memory>

namespace cc::demo
{
    extern std::shared_ptr<API> provider;
}
