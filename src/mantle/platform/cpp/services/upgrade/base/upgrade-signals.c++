// -*- c++ -*-
//==============================================================================
/// @file upgrade-signals.c++
/// @brief Upgrade service - signals
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-signals.h++"

namespace platform::upgrade
{
    core::signal::DataSignal<ScanProgress::Ref>
        signal_scan_progress("signal_scan_progress", true);

    core::signal::DataSignal<PackageInfo::Ref>
        signal_upgrade_available("upgrade_available", true);

    core::signal::DataSignal<PackageInfo::Ref>
        signal_upgrade_pending("upgrade_pending", true);

    core::signal::DataSignal<UpgradeProgress::Ref>
        signal_upgrade_progress("upgrade_progress", true);

}  // namespace platform::upgrade
