// -*- c++ -*-
//==============================================================================
/// @file multilogger-types.h++
/// @brief Logging service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "status/event.h++"
#include "types/getter.h++"
#include "types/value.h++"
#include "logging/sinks/sink.h++"
#include "logging/sinks/tabulardata.h++"

#include <optional>

namespace multilogger
{
    using SinkID = core::logging::SinkID;
    using SinkIDs = std::vector<SinkID>;
    using SinkType = core::logging::SinkType;
    using SinkTypes = std::vector<SinkType>;
    using ContractID = core::status::Event::ContractID;

    struct SinkSpec
    {
        SinkID sink_id;
        SinkType sink_type;
        bool permanent = false;
        std::string filename_template;
        core::dt::DateTimeInterval rotation_interval;
        bool use_local_time = true;
        core::status::Level min_level = core::status::Level::NONE;
        std::optional<ContractID> contract_id;
        core::logging::ColumnSpecs columns;
    };

    std::ostream &operator<<(std::ostream &stream, const SinkSpec &spec);

    using SinkSpecs = std::vector<SinkSpec>;
    using FieldNames = std::vector<std::string>;

    struct ListenerSpec
    {
        SinkID sink_id;
        core::status::Level min_level = core::status::Level::NONE;
        std::optional<ContractID> contract_id;
    };

    std::ostream &operator<<(std::ostream &stream, const ListenerSpec &spec);

    using EventSource = core::types::Getter<core::status::Event::ptr>;
}  // namespace multilogger
