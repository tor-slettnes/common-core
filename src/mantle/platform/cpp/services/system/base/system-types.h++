// -*- c++ -*-
//==============================================================================
/// @file system-types.h++
/// @brief System service - Data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "system.pb.h"          // Generated from `system.proto`

namespace platform::system
{
    using cc::system::ProductInfo;
    using cc::system::HostInfo;

    using cc::system::TimeSync;
    using cc::system::TimeConfig;

    using cc::system::TimeZoneConfig;
    using cc::system::TimeZoneSpec;
    using cc::system::TimeZoneInfo;

    using cc::system::CommandInvocation;
    using cc::system::CommandInvocationStatus;
    using cc::system::CommandInput;
    using cc::system::CommandResponse;
}
