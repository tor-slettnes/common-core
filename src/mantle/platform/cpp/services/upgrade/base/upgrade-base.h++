// -*- c++ -*-
//==============================================================================
/// @file upgrade-base.h++
/// @brief Upgrade service - abstract API
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "upgrade-types.h++"
#include "platform/provider.h++"

namespace upgrade
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
        //
        // This call returns immediately, without waiting for the results of the
        // scan.  To monitor progress and results of the scan connect to
        // `signal_scan_progress` and `signal_upgrade_available`, respectively.
        //
        // This call is not required for ongoing upgrade availability notifications.
        // By default, removable devices (e.g. USB drives) are scanned on insertion,
        // and online checks are performed at regular intervals if an Internet
        // connection is available.
        virtual PackageCatalogue scan(
            const PackageSource &source = {}) = 0;

        // Return information about all currently enabled upgrade sources.
        virtual PackageSources list_sources() const = 0;

        // Return information about available upgrade packages discovered during
        // a prior (implicit or explicit) scan of the specified package source
        // if specified, otherwise across all preconfigured/default sources.
        virtual PackageCatalogue list_available(
            const PackageSource &source = {}) const = 0;

        // Return information about the "best" available upgrade package
        // discovered during prior scans of the specified package sources, if
        // any; otherwise the best candidate discovered during prior scan of all
        // preconfigured/default package sources. This will normally be the
        // package with the highest version number, with local (VFS) sources
        // preferered over remote (URL).
        //
        // An empty pointer is returned if no applicable update is available.
        //
        // This information is also available by connecting to
        // `signal_upgrade_available`.
        virtual PackageInfo::ptr best_available(
            const PackageSource &source = {}) const = 0;

        // Install an upgrade from the specified package source if provided,
        // otherwise the current "best" package source based on prior scans.  To
        // perform an explicit scan, invoke `check()` before `install()`.
        //
        // This call returns as soon as installation is underway. To monitor the
        // progress of the upgrade process, connect to `signal_upgrade_pending`
        // and `signal_upgrade_progress`.
        virtual PackageInfo::ptr install(
            const PackageSource &source = {}) = 0;

        // Finalize a completed upgrade.  This clears `signal_upgrade_progress`,
        // and if the upgrade requires a system reboot, do so now.
        virtual void finalize() = 0;
    };

    //==========================================================================
    // Provider instance

    extern core::platform::ProviderProxy<ProviderInterface> upgrade;

}  // namespace upgrade
