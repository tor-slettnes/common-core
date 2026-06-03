// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.h++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/multilogger/protobuf/multilogger_types.pb.h"
#include "multilogger-types.h++"
#include "logging/telemetry/data.h++"
#include "logging/message/message.h++"
#include "protobuf-event-types.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Loggable

    void encode(const core::types::Loggable &native,
                platform::multilogger::protobuf::Loggable *proto) noexcept;

    core::types::Loggable::ptr decode_loggable(
        const platform::multilogger::protobuf::Loggable &proto,
        const std::string &default_host = {}) noexcept;

    //==========================================================================
    // Data

    void encode(const core::logging::Data &native,
                platform::multilogger::protobuf::Data *proto) noexcept;

    void decode(const platform::multilogger::protobuf::Data &proto,
                core::logging::Data *native) noexcept;

    //==========================================================================
    // Message

    void encode(const core::logging::Message &native,
                platform::multilogger::protobuf::Message *proto) noexcept;

    void decode(const platform::multilogger::protobuf::Message &proto,
                const std::string &default_host,
                core::logging::Message *native) noexcept;

    //==========================================================================
    // SinkID

    void encode(const platform::multilogger::SinkID &native,
                platform::multilogger::protobuf::SinkID *proto);

    void decode(const platform::multilogger::protobuf::SinkID &proto,
                platform::multilogger::SinkID *native);

    //==========================================================================
    // SinkSpec

    void encode(const platform::multilogger::SinkSpec &native,
                platform::multilogger::protobuf::SinkSpec *proto);

    void decode(const platform::multilogger::protobuf::SinkSpec &proto,
                platform::multilogger::SinkSpec *native);

    //==========================================================================
    // platform::multilogger::SinkSpecs <-> platform::multilogger::protobuf::SinkSpecs

    void encode(const platform::multilogger::SinkSpecs &native,
                platform::multilogger::protobuf::SinkSpecs *proto);

    void decode(const platform::multilogger::protobuf::SinkSpecs &proto,
                platform::multilogger::SinkSpecs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native,
                platform::multilogger::protobuf::ColumnSpec *proto);

    void decode(const platform::multilogger::protobuf::ColumnSpec &proto,
                core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::types::ValueType &native,
                platform::multilogger::protobuf::ColumnType *proto);

    void decode(const platform::multilogger::protobuf::ColumnType &proto,
                core::types::ValueType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const platform::multilogger::ListenerSpec &native,
                platform::multilogger::protobuf::ListenerSpec *proto);

    void decode(const platform::multilogger::protobuf::ListenerSpec &proto,
                platform::multilogger::ListenerSpec *native);

}  // namespace cc::protobuf
