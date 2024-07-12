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
    using cc::platform::system::ProductInfo;
    using cc::platform::system::HostInfo;

    using cc::platform::system::TimeSync;
    using cc::platform::system::TimeConfig;

    using cc::platform::system::TimeZoneConfig;
    using cc::platform::system::TimeZoneSpec;
    using cc::platform::system::TimeZoneInfo;

    using cc::platform::system::CommandInvocation;
    using cc::platform::system::CommandInvocationStatus;
    using cc::platform::system::CommandInput;
    using cc::platform::system::CommandResponse;
}
