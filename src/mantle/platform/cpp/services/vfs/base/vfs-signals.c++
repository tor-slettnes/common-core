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

    core::signal::MappingSignal<ContextRef> signal_context("context", true);
    core::signal::MappingSignal<ContextRef> signal_context_in_use("context_in_use", true);
}
