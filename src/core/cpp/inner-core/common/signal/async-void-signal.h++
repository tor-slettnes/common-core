/// -*- c++ -*-
//==============================================================================
/// @file async-void-signal.h++
/// @brief Value-less signal type with asynchronous emit
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "signal/void-signal.h++"

namespace cc::core::signal
{
    //==========================================================================
    /// @class AsyncVoidSignal
    /// @brief Signal without data, emitted in parallel to all slots

    class AsyncVoidSignal : public VoidSignal
    {
        using Super = VoidSignal;

    public:
        using Super::Super;

        std::size_t emit() override;
    };

}  // namespace cc::core::signal
