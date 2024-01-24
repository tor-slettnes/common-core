// -*- c++ -*-
//==============================================================================
/// @file demo-zmq-common.h++
/// @brief Demo ZeroMQ implementation - common declarations
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "demo-api.h++"

namespace cc::demo::zmq
{
    // Name for looking up settings for "command" and "message" channels
    constexpr auto CHANNEL_NAME    = "Demo";

    // Interface name to use for RPC calls
    constexpr auto INTERFACE_NAME  = "DemoService";

    // Method names within this interface
    constexpr auto METHOD_SAY_HELLO = "say_hello";
    constexpr auto METHOD_GET_CURRENT_TIME = "get_current_time";
    constexpr auto METHOD_START_TICKING = "start_ticking";
    constexpr auto METHOD_STOP_TICKING = "stop_ticking";
}  // namespace cc::demo::zmq
