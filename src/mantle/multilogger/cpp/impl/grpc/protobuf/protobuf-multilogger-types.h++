// -*- c++ -*-
//==============================================================================
/// @file protobuf-multilogger-types.h++
/// @brief conversions to/from Protocol Buffer messages for MultiLogger application
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "multilogger-types.h++"
#include "multilogger.pb.h"

namespace protobuf
{
    //==========================================================================
    // SinkID

    void encode(const multilogger::SinkID &native, cc::multilogger::SinkID *proto);
    void decode(const cc::multilogger::SinkID &proto, multilogger::SinkID *native);

    //==========================================================================
    // SinkSpec

    void encode(const multilogger::SinkSpec &native, cc::multilogger::SinkSpec *proto);
    void decode(const cc::multilogger::SinkSpec &proto, multilogger::SinkSpec *native);

    //==========================================================================
    // multilogger::SinkSpecs <-> cc::multilogger::SinkSpecs

    void encode(const multilogger::SinkSpecs &native, cc::multilogger::SinkSpecs *proto);
    void decode(const cc::multilogger::SinkSpecs &proto, multilogger::SinkSpecs *native);

    //==========================================================================
    // ColumnSpec

    void encode(const core::logging::ColumnSpec &native, cc::multilogger::ColumnSpec *proto);
    void decode(const cc::multilogger::ColumnSpec &proto, core::logging::ColumnSpec *native);

    //==========================================================================
    // ColumnType

    void encode(const core::types::ValueType &native, cc::multilogger::ColumnType *proto);
    void decode(const cc::multilogger::ColumnType &proto, core::types::ValueType *native);

    //==========================================================================
    // ListenerSpec

    void encode(const multilogger::ListenerSpec &native, cc::multilogger::ListenerSpec *proto);
    void decode(const cc::multilogger::ListenerSpec &proto, multilogger::ListenerSpec *native);


}  // namespace protobuf
