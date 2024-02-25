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

namespace idl
{
    //==========================================================================
    // Encode/decode Boolean value

    void encode(bool native,
                CC::Common::BoolValue *idl) noexcept;

    void decode(const CC::Common::BoolValue &idl,
                bool *native) noexcept;

    //==========================================================================
    // Encode/decode Unsigned Integer value

    void encode(shared::types::largest_uint native,
                CC::Common::UnsignedValue *idl) noexcept;

    void decode(const CC::Common::UnsignedValue &idl,
                shared::types::largest_uint *native) noexcept;

    //==========================================================================
    // Encode/decode Signed Integer value

    void encode(shared::types::largest_sint native,
                CC::Common::SignedValue *idl) noexcept;

    void decode(const CC::Common::SignedValue &idl,
                shared::types::largest_sint *native) noexcept;

    //==========================================================================
    // Encode/decode real value

    void encode(double native,
                CC::Common::RealValue *idl) noexcept;

    void decode(const CC::Common::RealValue &idl,
                double *native) noexcept;

    //==========================================================================
    // Encode/decode Complex value

    void encode(const shared::types::complex &native,
                CC::Common::ComplexValue *idl) noexcept;

    void decode(const CC::Common::ComplexValue &idl,
                shared::types::complex *native) noexcept;

    //==========================================================================
    // Encode/decode Timestamp

    void encode(const shared::dt::TimePoint &native,
                CC::Common::Timestamp *idl) noexcept;

    void decode(const CC::Common::Timestamp &idl,
                shared::dt::TimePoint *native) noexcept;

    //==========================================================================
    // Encode/decode Duration

    void encode(const shared::dt::Duration &native,
                CC::Common::Duration *idl) noexcept;

    void decode(const CC::Common::Duration &idl,
                shared::dt::Duration *native) noexcept;

    //==========================================================================
    // Encode/decode String value

    void encode(std::string native,
                CC::Common::StringValue *idl) noexcept;

    void decode(const CC::Common::StringValue &idl,
                std::string *native) noexcept;

}  // namespace idl
