// -*- c++ -*-
//==============================================================================
/// @file upgrade-base.h++
/// @brief Upgrade service - abstract API
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "platform/provider.h++"

namespace platform::upgrade
{
    //==========================================================================
    // Provider

    class ProviderInterface : public core::platform::Provider
    {
        using This = ProviderInterface;
        using Super = core::platform::Provider;

    public:
        using Super::Super;

        // Explicit scan for available upgrade packages in the specified package
        // source if provided, otherwise in the preconfigured/default locations.
        // Only packages for a matching product name and with a version number
        // that's "better" than the currently installed release are considered.
        //
        // This function returns immediately.  Connect to `signal_scan_progress`
        // and `signal_upgrade_available` to monitor the progress and result of
        // the check.
        //
        // This call is not required for ongoing upgrade availability notifications.
        // By default, removable devices (e.g. USB drives) are scanned on insertion,
        // and online checks are performed at regular intervals if an Internet
        // connection is available.
        virtual void scan(const PackageSource &source = {}) = 0;

        // Return information about any available upgrade package based on prior
        // scans. This call returns immediately.
        //
        // This information is also available by connecting to
        // `signal_upgrade_available`.
        virtual PackageInfo::Ref get_available() const = 0;

        // Install an upgrade from the specified package source if provided,
        // otherwise the current "best" package source based on prior scans.  To
        // perform an explicit scan, invoke `check()` before `install()`.
        //
        // This call returns immediately. use the `watch()` streaming call
        // to monitor the progress and result of the upgrade.
        virtual PackageInfo::Ref install(const PackageSource &source = {}) = 0;

        // Finalize a completed upgrade.  This clears `signal_upgrade_progress`,
        // and if the upgrade requires a system reboot, do so now.
        virtual void finalize() = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProviderInterface> upgrade;

}  // namespace platform::upgrade
