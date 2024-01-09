/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-common.c++
/// @brief Encode/decode routines for common IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-common.h++"  // generated from common_types.idl

namespace cc::idl
{
    //==========================================================================
    // Encode/decode Boolean value

    void encode(bool native,
                CC::Shared::BoolValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Shared::BoolValue &idl,
                bool *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Unsigned Integer value

    void encode(cc::types::largest_uint native,
                CC::Shared::UnsignedValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Shared::UnsignedValue &idl,
                cc::types::largest_uint *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Signed Integer value

    void encode(cc::types::largest_sint native,
                CC::Shared::SignedValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Shared::SignedValue &idl,
                cc::types::largest_sint *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode real value

    void encode(double native,
                CC::Shared::RealValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Shared::RealValue &idl,
                double *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Complex value

    void encode(const cc::types::complex &native,
                CC::Shared::ComplexValue *idl) noexcept
    {
        idl->real(native.real());
        idl->imag(native.imag());
    }

    void decode(const CC::Shared::ComplexValue &idl,
                cc::types::complex *native) noexcept
    {
        native->real(idl.real());
        native->imag(idl.imag());
    }

    //==========================================================================
    // Encode/decode Timestamp

    void encode(const cc::dt::TimePoint &native,
                CC::Shared::Timestamp *idl) noexcept
    {
        timespec tspec = cc::dt::to_timespec(native);
        idl->seconds(tspec.tv_sec);
        idl->nanoseconds(tspec.tv_nsec);
    }

    void decode(const CC::Shared::Timestamp &idl,
                cc::dt::TimePoint *native) noexcept
    {
        *native = cc::dt::to_timepoint(idl.seconds(), idl.nanoseconds());
    }

    //==========================================================================
    // Encode/decode Duration

    void encode(const cc::dt::Duration &native,
                CC::Shared::Duration *idl) noexcept
    {
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(native);
        if (secs > native)
            secs -= std::chrono::seconds(1);
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(native - secs);
        idl->seconds(static_cast<std::uint64_t>(secs.count()));
        idl->nanoseconds(static_cast<std::uint32_t>(nanos.count()));
    }

    void decode(const CC::Shared::Duration &idl,
                cc::dt::Duration *native) noexcept
    {
        *native = std::chrono::duration_cast<cc::dt::Duration>(
            std::chrono::seconds(idl.seconds()) +
            std::chrono::nanoseconds(idl.nanoseconds()));
    }

    //==========================================================================
    // Encode/decode String value

    void encode(std::string native,
                CC::Shared::StringValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Shared::StringValue &idl,
                std::string *native) noexcept
    {
        *native = idl.value();
    }

}  // namespace cc::idl
