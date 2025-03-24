// -*- c++ -*-
//==============================================================================
/// @file upgrade-signals.h++
/// @brief Upgrade service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "thread/signaltemplate.h++"

namespace upgrade
{
    extern core::signal::DataSignal<ScanProgress::ptr> signal_scan_progress;
    extern core::signal::DataSignal<PackageInfo::ptr> signal_upgrade_available;
    extern core::signal::DataSignal<PackageInfo::ptr> signal_upgrade_pending;
    extern core::signal::DataSignal<UpgradeProgress::ptr> signal_upgrade_progress;
}  // namespace upgrade
