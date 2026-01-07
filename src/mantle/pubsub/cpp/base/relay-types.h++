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

namespace pubsub
{
    using Topic = std::string;
    using TopicSet = std::unordered_set<Topic>;
    using Payload = core::types::Value;

    using MessageItem = std::pair<Topic, Payload>;
    using MessageSource = core::types::Getter<MessageItem>;
    using MessageHandler = std::function<void(Topic, Payload)>;

    //--------------------------------------------------------------------------
    // Signals

    extern core::signal::MappingSignal<core::types::Value> signal_publication;

    //--------------------------------------------------------------------------
    // Log scope
    define_log_scope("relay");

}  // namespace pubsub

