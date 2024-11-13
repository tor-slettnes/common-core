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
    // SinkType

    void encode(const logger::SinkType &native, cc::logger::SinkType *proto);
    void decode(const cc::logger::SinkType &proto, logger::SinkType *native);

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
    // logger::SinkIDs <-> cc::logger::SinkSpecs

    void encode(const logger::SinkIDs &native, cc::logger::SinkSpecs *proto);
    void decode(const cc::logger::SinkSpecs &proto, logger::SinkIDs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::logger::ColumnSpec *proto);
    void decode(const cc::logger::ColumnSpec &proto, core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::logging::ColumnType &native, cc::logger::ColumnType *proto);
    void decode(const cc::logger::ColumnType &proto, core::logging::ColumnType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const logger::ListenerSpec &native, cc::logger::ListenerSpec *proto);
    void decode(const cc::logger::ListenerSpec &proto, logger::ListenerSpec *native);

    //==========================================================================
    // FieldNames

    void encode(const logger::FieldNames &native, cc::logger::FieldNames *proto);
    void decode(const cc::logger::FieldNames &proto, logger::FieldNames *native);

}  // namespace protobuf
