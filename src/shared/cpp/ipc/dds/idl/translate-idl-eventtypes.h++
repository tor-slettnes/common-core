/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.h++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "event-types.hpp"  // Generated from `event-types.idl`
#include "status/event.h++"
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
    // Flow
    void encode(const cc::status::Flow &native, CC::Status::Flow *idl) noexcept;
    void decode(const CC::Status::Flow &idl, cc::status::Flow *native) noexcept;

    //==========================================================================
    // Event
    void encode(const cc::status::Event &native, CC::Status::Event *idl) noexcept;
    void decode(const CC::Status::Event &idl, cc::status::Event *native) noexcept;
    cc::status::Event decoded_event(const CC::Status::Event &idl) noexcept;

    //==========================================================================
    // LogMessage
    void encode(const cc::logging::Message &native, CC::Status::LogMessage *idl) noexcept;
    cc::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept;

}  // namespace cc::idl
