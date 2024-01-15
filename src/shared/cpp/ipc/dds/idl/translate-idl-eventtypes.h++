/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.h++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event-types.hpp"  // Generated from `event-types.idl`
#include "status/error.h++"
#include "logging/message/message.h++"
#include "translate-idl-common.h++"
#include "translate-idl-variant.h++"

namespace cc::idl
{
    //==========================================================================
    // Domain
    void encode(const cc::status::Domain &native, CC::Status::Domain *idl) noexcept;
    void decode(const CC::Status::Domain &idl, cc::status::Domain *native) noexcept;

    //==========================================================================
    // Level
    void encode(const cc::status::Level &native, CC::Status::Level *idl) noexcept;
    void decode(const CC::Status::Level &idl, cc::status::Level *native) noexcept;

    //==========================================================================
    // Event
    void encode(const cc::status::Event &native, CC::Status::Event *idl) noexcept;
    void decode(const CC::Status::Event &idl, cc::status::Event *native) noexcept;
    cc::status::Event decoded_event(const CC::Status::Event &idl) noexcept;

    //==========================================================================
    // LogMessage
    void encode(const cc::logging::Message &native, CC::Status::LogMessage *idl) noexcept;
    cc::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept;

    //==========================================================================
    // ErrorEvent
    void encode(const cc::status::Error &native, CC::Status::ErrorEvent *idl) noexcept;
    void decode(const CC::Status::ErrorEvent &idl, cc::status::Error *native) noexcept;
    cc::status::Error decoded_error(const CC::Status::ErrorEvent &idl) noexcept;
}  // namespace cc::idl
