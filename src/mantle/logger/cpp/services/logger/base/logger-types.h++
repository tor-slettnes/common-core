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
#include "types/valuemap.h++"
#include "logging/sinks/tabulardatasink.h++"

#include <optional>

namespace logger
{
    using ContractName = std::string;
    using ContractTemplate = core::logging::ColumnDefaults;
    using ContractsMap = core::types::ValueMap<ContractName, ContractTemplate>;

    struct Filter
    {
        core::status::Level min_level;
        std::optional<ContractName> contract_name;
    };

    using EventQueue = core::types::BlockingQueue<core::status::Event::ptr>;
}  // namespace logger
