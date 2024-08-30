// -*- c++ -*-
//==============================================================================
/// @file upgrade-signals.h++
/// @brief Upgrade service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "thread/signaltemplate.h++"

namespace platform::upgrade
{
    extern core::signal::DataSignal<ScanProgress::Ref> signal_scan_progress;
    extern core::signal::DataSignal<PackageInfo::Ref> signal_upgrade_available;
    extern core::signal::DataSignal<UpgradeProgress::Ref> signal_upgrade_progress;
}  // namespace platform::upgrade
