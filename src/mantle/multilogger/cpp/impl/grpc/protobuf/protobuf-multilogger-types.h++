// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.h++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger.pb.h"
#include "multilogger-types.h++"
#include "logging/telemetry/data.h++"
#include "logging/message/message.h++"
#include "protobuf-event-types.h++"

namespace protobuf
{
    //==========================================================================
    // Loggable

    void encode(const core::types::Loggable &native,
                cc::platform::multilogger::Loggable *proto) noexcept;

    core::types::Loggable::ptr decode_loggable(
        const cc::platform::multilogger::Loggable &proto,
        const std::string &default_host) noexcept;

    //==========================================================================
    // Data

    void encode(const core::logging::Data &native,
                cc::platform::multilogger::Data *proto) noexcept;

    void decode(const cc::platform::multilogger::Data &proto,
                core::logging::Data *native) noexcept;

    //==========================================================================
    // Message

    void encode(const core::logging::Message &native,
                cc::platform::multilogger::Message *proto) noexcept;

    void decode(const cc::platform::multilogger::Message &proto,
                const std::string &default_host,
                core::logging::Message *native) noexcept;

    //==========================================================================
    // SinkID

    void encode(const multilogger::SinkID &native, cc::platform::multilogger::SinkID *proto);
    void decode(const cc::platform::multilogger::SinkID &proto, multilogger::SinkID *native);

    //==========================================================================
    // SinkSpec

    void encode(const multilogger::SinkSpec &native, cc::platform::multilogger::SinkSpec *proto);
    void decode(const cc::platform::multilogger::SinkSpec &proto, multilogger::SinkSpec *native);

    //==========================================================================
    // multilogger::SinkSpecs <-> cc::platform::multilogger::SinkSpecs

    void encode(const multilogger::SinkSpecs &native, cc::platform::multilogger::SinkSpecs *proto);
    void decode(const cc::platform::multilogger::SinkSpecs &proto, multilogger::SinkSpecs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::platform::multilogger::ColumnSpec *proto);
    void decode(const cc::platform::multilogger::ColumnSpec &proto, core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::types::ValueType &native, cc::platform::multilogger::ColumnType *proto);
    void decode(const cc::platform::multilogger::ColumnType &proto, core::types::ValueType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const multilogger::ListenerSpec &native, cc::platform::multilogger::ListenerSpec *proto);
    void decode(const cc::platform::multilogger::ListenerSpec &proto, multilogger::ListenerSpec *native);

}  // namespace protobuf
