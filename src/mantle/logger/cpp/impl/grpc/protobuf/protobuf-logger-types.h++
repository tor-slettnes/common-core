// -*- c++ -*-
//==============================================================================
/// @file protobuf-logger-types.h++
/// @brief conversions to/from Protocol Buffer messages for Logger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logger-types.h++"
#include "logger.pb.h"

namespace protobuf
{
    //==========================================================================
    // SinkID

    void encode(const logger::SinkID &native, cc::logger::SinkID *proto);
    void decode(const cc::logger::SinkID &proto, logger::SinkID *native);

    //==========================================================================
    // SinkSpec

    void encode(const logger::SinkSpec &native, cc::logger::SinkSpec *proto);
    void decode(const cc::logger::SinkSpec &proto, logger::SinkSpec *native);

    //==========================================================================
    // logger::SinkSpecs <-> cc::logger::SinkSpecs

    void encode(const logger::SinkSpecs &native, cc::logger::SinkSpecs *proto);
    void decode(const cc::logger::SinkSpecs &proto, logger::SinkSpecs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::logger::ColumnSpec *proto);
    void decode(const cc::logger::ColumnSpec &proto, core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::types::ValueType &native, cc::logger::ColumnType *proto);
    void decode(const cc::logger::ColumnType &proto, core::types::ValueType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const logger::ListenerSpec &native, cc::logger::ListenerSpec *proto);
    void decode(const cc::logger::ListenerSpec &proto, logger::ListenerSpec *native);


}  // namespace protobuf
