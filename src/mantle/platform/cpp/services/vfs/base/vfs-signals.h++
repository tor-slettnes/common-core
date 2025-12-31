// -*- c++ -*-
//==============================================================================
/// @file vfs-signals.h++
/// @brief VFS service - signals
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "vfs-context.h++"
#include "thread/signaltemplate.h++"

namespace vfs
{
    //==========================================================================
    // Signals

    // Emitted whenever a context becomes available/unavailable (e.g. when a USB
    // drive is inserted/removed).
    extern core::signal::MappingSignal<Context::ptr> signal_context;

    // Emitted whenever a context changes status, e.g., is opened or closed.
    extern core::signal::MappingSignal<Context::ptr> signal_context_in_use;
}  // namespace vfs
