/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-eventtypes.h++
/// @brief Encode/decode routines for common IDL error/status types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "event-types.hpp"  // Generated from `event-types.idl`
#include "status/error.h++"
#include "logging/message/message.h++"
#include "translate-idl-common.h++"
#include "translate-idl-variant.h++"

namespace idl
{
    //==========================================================================
    // Domain
    void encode(const core::status::Domain &native, CC::Status::Domain *idl) noexcept;
    void decode(const CC::Status::Domain &idl, core::status::Domain *native) noexcept;

    CC::Status::Domain encoded(const core::status::Domain &native) noexcept;
    core::status::Domain decoded_domain(const CC::Status::Domain &idl) noexcept;

    //==========================================================================
    // Level

    void encode(const core::status::Level &native, CC::Status::Level *idl) noexcept;
    void decode(const CC::Status::Level &idl, core::status::Level *native) noexcept;

    CC::Status::Level encoded(const core::status::Level &native) noexcept;
    core::status::Level decoded_level(const CC::Status::Level &idl) noexcept;

    //==========================================================================
    // Event
    void encode(const core::status::Event &native, CC::Status::Event *idl) noexcept;

    //==========================================================================
    // Error
    void encode(const core::status::Error &native, CC::Status::Error *idl) noexcept;
    void decode(const CC::Status::Error &idl, core::status::Error *native) noexcept;
    core::status::Error decoded_error(const CC::Status::Error &idl) noexcept;

    //==========================================================================
    // LogMessage
    void encode(const core::logging::Message &native, CC::Status::LogMessage *idl) noexcept;
    void decode(const CC::Status::LogMessage &idl, core::logging::Message *native) noexcept;
    core::logging::Message decoded_logmessage(const CC::Status::LogMessage &idl) noexcept;

}  // namespace idl
