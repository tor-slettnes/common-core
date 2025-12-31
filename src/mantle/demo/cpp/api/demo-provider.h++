// -*- c++ -*-
//==============================================================================
/// @file demo-provider.h++
/// @brief Example App - interface
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "demo-api.h++"      // To invoke functionalty directly
#include "demo-signals.h++"  // To emit/receive demo data updates

#include <memory>

namespace demo
{
    extern std::shared_ptr<API> provider;
}
