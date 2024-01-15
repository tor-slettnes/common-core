/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-common.h++
/// @brief Encode/decode routines for common IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "common-types.hpp"  // generated from `common_types.idl`
#include "chrono/date-time.h++"
#include "types/value.h++"

namespace cc::idl
{
    //==========================================================================
    // Encode/decode Boolean value

    void encode(bool native,
                CC::Shared::BoolValue *idl) noexcept;

    void decode(const CC::Shared::BoolValue &idl,
                bool *native) noexcept;

    //==========================================================================
    // Encode/decode Unsigned Integer value

    void encode(cc::types::largest_uint native,
                CC::Shared::UnsignedValue *idl) noexcept;

    void decode(const CC::Shared::UnsignedValue &idl,
                cc::types::largest_uint *native) noexcept;

    //==========================================================================
    // Encode/decode Signed Integer value

    void encode(cc::types::largest_sint native,
                CC::Shared::SignedValue *idl) noexcept;

    void decode(const CC::Shared::SignedValue &idl,
                cc::types::largest_sint *native) noexcept;

    //==========================================================================
    // Encode/decode real value

    void encode(double native,
                CC::Shared::RealValue *idl) noexcept;

    void decode(const CC::Shared::RealValue &idl,
                double *native) noexcept;

    //==========================================================================
    // Encode/decode Complex value

    void encode(const cc::types::complex &native,
                CC::Shared::ComplexValue *idl) noexcept;

    void decode(const CC::Shared::ComplexValue &idl,
                cc::types::complex *native) noexcept;

    //==========================================================================
    // Encode/decode Timestamp

    void encode(const cc::dt::TimePoint &native,
                CC::Shared::Timestamp *idl) noexcept;

    void decode(const CC::Shared::Timestamp &idl,
                cc::dt::TimePoint *native) noexcept;

    //==========================================================================
    // Encode/decode Duration

    void encode(const cc::dt::Duration &native,
                CC::Shared::Duration *idl) noexcept;

    void decode(const CC::Shared::Duration &idl,
                cc::dt::Duration *native) noexcept;

    //==========================================================================
    // Encode/decode String value

    void encode(std::string native,
                CC::Shared::StringValue *idl) noexcept;

    void decode(const CC::Shared::StringValue &idl,
                std::string *native) noexcept;

}  // namespace cc::idl
