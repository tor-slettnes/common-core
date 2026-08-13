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

#include <iostream>

namespace cc::platform::pubsub
{
    using Topic = std::string;
    using TopicSet = std::unordered_set<Topic>;
    using Payload = core::types::Value;

    using MessageItem = std::pair<Topic, Payload>;
    using MessageSource = core::types::Getter<MessageItem>;
    using MessageHandler = std::function<void(Topic, Payload)>;

    using KeyPath = std::vector<std::string>;
    using KeyPaths = std::vector<KeyPath>;

    struct ReplayPolicy
    {
        bool replay_latest;
        KeyPaths key_paths;
    };
    std::ostream &operator<<(std::ostream &stream, const ReplayPolicy &policy);

    using ReplayPolicyMap = core::types::ValueMap<Topic, ReplayPolicy>;
    using Payloads = std::vector<Payload>;
    using Snapshot = core::types::ValueMap<Topic, Payloads>;

    constexpr auto KEY_PATH_DELIMITER = "/";

    //--------------------------------------------------------------------------
    // Signals

    extern core::signal::MappingSignal<core::types::Value> signal_publication;

    //--------------------------------------------------------------------------
    // Log scope
    define_log_scope("relay");

}  // namespace cc::platform::pubsub
