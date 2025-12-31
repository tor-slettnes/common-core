// -*- c++ -*-
//==============================================================================
/// @file multilogger-types.h++
/// @brief Logging service - data types
/// @author Tor Slettnes
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
    using core::types::Loggable;
    using core::logging::SinkID;
    using core::logging::SinkType;

    struct SinkSpec
    {
        SinkID sink_id;
        SinkType sink_type;
        // bool permanent = false;
        std::optional<Loggable::ContractID> contract_id;
        core::status::Level min_level = core::status::Level::NONE;
        std::string filename_template;
        core::dt::DateTimeInterval rotation_interval;
        bool use_local_time = true;
        bool compress_after_use = true;
        core::dt::DateTimeInterval expiration_interval;
        core::logging::ColumnSpecs columns;
    };

    std::ostream &operator<<(std::ostream &stream, const SinkSpec &spec);

    using SinkIDs = std::vector<SinkID>;
    using SinkTypes = std::vector<SinkType>;
    using SinkSpecs = std::vector<SinkSpec>;
    using FieldNames = std::vector<std::string>;
    using LogSource = core::types::Getter<core::types::Loggable::ptr>;

    struct ListenerSpec
    {
        SinkID sink_id;
        core::status::Level min_level = core::status::Level::NONE;
        std::optional<Loggable::ContractID> contract_id;
        std::set<std::string> hosts;
        std::set<std::string> applications;
    };

    std::ostream &operator<<(std::ostream &stream, const ListenerSpec &spec);

}  // namespace multilogger
