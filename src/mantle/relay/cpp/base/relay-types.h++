// -*- c++ -*-
//==============================================================================
/// @file relay-types.h++
/// @brief Relay common definitions
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "thread/signaltemplate.h++"
#include "logging/logging.h++"
#include "types/value.h++"
#include "types/getter.h++"

namespace relay
{
    using Topic = std::string;
    using Payload = core::types::Value;
    using MessageData = std::pair<Topic, Payload>;
    using MessageSource = core::types::Getter<MessageData>;
    using MessageReceiver = std::function<void(Topic, Payload)>;

    //--------------------------------------------------------------------------
    // Signals

    extern core::signal::MappingSignal<core::types::Value> signal_message;

    //--------------------------------------------------------------------------
    // Log scope
    define_log_scope("relay");

}  // namespace relay

