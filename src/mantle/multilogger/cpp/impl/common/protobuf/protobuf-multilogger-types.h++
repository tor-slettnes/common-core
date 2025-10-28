// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.h++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "cc/platform/multilogger/protobuf/multilogger_types.pb.h"
#include "multilogger-types.h++"
#include "logging/telemetry/data.h++"
#include "logging/message/message.h++"
#include "protobuf-event-types.h++"

namespace protobuf
{
    //==========================================================================
    // Loggable

    void encode(const core::types::Loggable &native,
                cc::platform::multilogger::protobuf::Loggable *proto) noexcept;

    core::types::Loggable::ptr decode_loggable(
        const cc::platform::multilogger::protobuf::Loggable &proto,
        const std::string &default_host = {}) noexcept;

    //==========================================================================
    // Data

    void encode(const core::logging::Data &native,
                cc::platform::multilogger::protobuf::Data *proto) noexcept;

    void decode(const cc::platform::multilogger::protobuf::Data &proto,
                core::logging::Data *native) noexcept;

    //==========================================================================
    // Message

    void encode(const core::logging::Message &native,
                cc::platform::multilogger::protobuf::Message *proto) noexcept;

    void decode(const cc::platform::multilogger::protobuf::Message &proto,
                const std::string &default_host,
                core::logging::Message *native) noexcept;

    //==========================================================================
    // SinkID

    void encode(const multilogger::SinkID &native, cc::platform::multilogger::protobuf::SinkID *proto);
    void decode(const cc::platform::multilogger::protobuf::SinkID &proto, multilogger::SinkID *native);

    //==========================================================================
    // SinkSpec

    void encode(const multilogger::SinkSpec &native, cc::platform::multilogger::protobuf::SinkSpec *proto);
    void decode(const cc::platform::multilogger::protobuf::SinkSpec &proto, multilogger::SinkSpec *native);

    //==========================================================================
    // multilogger::SinkSpecs <-> cc::platform::multilogger::protobuf::SinkSpecs

    void encode(const multilogger::SinkSpecs &native, cc::platform::multilogger::protobuf::SinkSpecs *proto);
    void decode(const cc::platform::multilogger::protobuf::SinkSpecs &proto, multilogger::SinkSpecs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::platform::multilogger::protobuf::ColumnSpec *proto);
    void decode(const cc::platform::multilogger::protobuf::ColumnSpec &proto, core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::types::ValueType &native, cc::platform::multilogger::protobuf::ColumnType *proto);
    void decode(const cc::platform::multilogger::protobuf::ColumnType &proto, core::types::ValueType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const multilogger::ListenerSpec &native, cc::platform::multilogger::protobuf::ListenerSpec *proto);
    void decode(const cc::platform::multilogger::protobuf::ListenerSpec &proto, multilogger::ListenerSpec *native);

}  // namespace protobuf
