// -*- c++ -*-
//==============================================================================
/// @file logger-types.h++
/// @brief Logging service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/event.h++"
#include "thread/blockingqueue.h++"
#include "types/value.h++"
#include "logging/sinks/sink.h++"

#include <optional>

namespace logger
{
    using SinkID = std::string;
    using ContractID = core::status::Event::ContractID;

    // Sink types that can be added by remote client
    enum class SinkType
    {
        UNSPECIFIED,
        SYSLOG,
        LOGFILE,
        JSON,
        CSV,
        DB
    };

    struct SinkSpec
    {
        SinkID sink_id;
        SinkType sink_type = SinkType::UNSPECIFIED;
        bool persistent = false;
        std::string filename_template;
        core::dt::DateTimeInterval rotation_interval;
        bool use_local_time = true;
        core::status::Level min_level = core::status::Level::NONE;
        std::optional<ContractID> contract_id;
        core::types::TaggedValueList fields;
    };

    using SinkSpecs = std::vector<SinkSpec>;
    using FieldNames = std::vector<std::string>;

    struct ListenerSpec
    {
        SinkID sink_id;
        core::status::Level min_level = core::status::Level::NONE;
        std::optional<ContractID> contract_id;
    };

    using EventQueue = core::types::BlockingQueue<core::status::Event::ptr>;
}  // namespace logger
