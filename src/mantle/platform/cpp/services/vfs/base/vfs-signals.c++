// -*- c++ -*-
//==============================================================================
/// @file vfs-signals.c++
/// @brief VFS service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "vfs-signals.h++"

namespace platform::vfs
{
    //==========================================================================
    // Signals

    core::signal::MappingSignal<Context::ptr> signal_context("context", true);
    core::signal::MappingSignal<Context::ptr> signal_context_in_use("context_in_use", true);
}  // namespace platform::vfs
