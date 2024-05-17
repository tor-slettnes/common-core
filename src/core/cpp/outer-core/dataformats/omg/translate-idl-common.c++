/// -*- c++ -*-
//==============================================================================
/// @file translate-idl-common.c++
/// @brief Encode/decode routines for common IDL types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "translate-idl-common.h++"  // generated from common_types.idl

namespace core::idl
{
    //==========================================================================
    // Encode/decode Boolean value

    void encode(bool native,
                CC::Common::BoolValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Common::BoolValue &idl,
                bool *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Unsigned Integer value

    void encode(core::types::largest_uint native,
                CC::Common::UnsignedValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Common::UnsignedValue &idl,
                core::types::largest_uint *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Signed Integer value

    void encode(core::types::largest_sint native,
                CC::Common::SignedValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Common::SignedValue &idl,
                core::types::largest_sint *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode real value

    void encode(double native,
                CC::Common::RealValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Common::RealValue &idl,
                double *native) noexcept
    {
        *native = idl.value();
    }

    //==========================================================================
    // Encode/decode Complex value

    void encode(const core::types::complex &native,
                CC::Common::ComplexValue *idl) noexcept
    {
        idl->real(native.real());
        idl->imag(native.imag());
    }

    void decode(const CC::Common::ComplexValue &idl,
                core::types::complex *native) noexcept
    {
        native->real(idl.real());
        native->imag(idl.imag());
    }

    //==========================================================================
    // Encode/decode Timestamp

    void encode(const core::dt::TimePoint &native,
                CC::Common::Timestamp *idl) noexcept
    {
        timespec tspec = core::dt::to_timespec(native);
        idl->seconds(tspec.tv_sec);
        idl->nanoseconds(tspec.tv_nsec);
    }

    void decode(const CC::Common::Timestamp &idl,
                core::dt::TimePoint *native) noexcept
    {
        *native = core::dt::to_timepoint(idl.seconds(), idl.nanoseconds());
    }

    //==========================================================================
    // Encode/decode Duration

    void encode(const core::dt::Duration &native,
                CC::Common::Duration *idl) noexcept
    {
        auto secs = std::chrono::duration_cast<std::chrono::seconds>(native);
        if (secs > native)
            secs -= std::chrono::seconds(1);
        auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(native - secs);
        idl->seconds(static_cast<std::uint64_t>(secs.count()));
        idl->nanoseconds(static_cast<std::uint32_t>(nanos.count()));
    }

    void decode(const CC::Common::Duration &idl,
                core::dt::Duration *native) noexcept
    {
        *native = std::chrono::duration_cast<core::dt::Duration>(
            std::chrono::seconds(idl.seconds()) +
            std::chrono::nanoseconds(idl.nanoseconds()));
    }

    //==========================================================================
    // Encode/decode String value

    void encode(std::string native,
                CC::Common::StringValue *idl) noexcept
    {
        idl->value(native);
    }

    void decode(const CC::Common::StringValue &idl,
                std::string *native) noexcept
    {
        *native = idl.value();
    }

}  // namespace core::idl
