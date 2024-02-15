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

namespace idl
{
    //==========================================================================
    // Domain
    void encode(const shared::status::Domain &native, CC::Status::Domain *idl) noexcept;
    void decode(const CC::Status::Domain &idl, shared::status::Domain *native) noexcept;

    //==========================================================================
    // Level
    void encode(const shared::status::Level &native, CC::Status::Level *idl) noexcept;
    void decode(const CC::Status::Level &idl, shared::status::Level *native) noexcept;

    //==========================================================================
    // Flow
    void encode(const shared::status::Flow &native, CC::Status::Flow *idl) noexcept;
    void decode(const CC::Status::Flow &idl, shared::status::Flow *native) noexcept;

    //==========================================================================
    // Event
    void encode(const shared::status::Event &native, CC::Status::Event *idl) noexcept;
    void decode(const CC::Status::Event &idl, shared::status::Event *native) noexcept;
    shared::status::Event decoded_event(const CC::Status::Event &idl) noexcept;

    //==========================================================================
    // LogMessage
    void encode(const shared::logging::Message &native, CC::Status::LogMessage *idl) noexcept;
    shared::logging::Message decoded_logmessage(CC::Status::LogMessage idl) noexcept;

}  // namespace idl
