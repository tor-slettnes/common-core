/// -*- c++ -*-
//==============================================================================
/// @file protobuf-upgrade-types.h++
/// @brief Encode/decode routines for upgrade ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade.pb.h"  // generated from `variant.proto`
#include "upgrade-base.h++"

namespace protobuf
{
    //==========================================================================
    // Package Source

    void encode(const ::platform::upgrade::PackageSource &native,
                ::cc::platform::upgrade::PackageSource *msg);

    void decode(const ::cc::platform::upgrade::PackageSource &msg,
                ::platform::upgrade::PackageSource *native);

    //==========================================================================
    // Package Sources

    void encode(const ::platform::upgrade::PackageSources &native,
                ::cc::platform::upgrade::PackageSources *msg);

    void decode(const ::cc::platform::upgrade::PackageSources &msg,
                ::platform::upgrade::PackageSources *native);

    //==========================================================================
    // Package Information

    void encode(const ::platform::upgrade::PackageInfo &native,
                ::cc::platform::upgrade::PackageInfo *msg);

    void decode(const ::cc::platform::upgrade::PackageInfo &msg,
                ::platform::upgrade::PackageInfo *native);

    //==========================================================================
    // Package Catalogue

    void encode(const ::platform::upgrade::PackageCatalogue &native,
                ::cc::platform::upgrade::PackageCatalogue *msg);

    void decode(const ::cc::platform::upgrade::PackageCatalogue &msg,
                ::platform::upgrade::PackageCatalogue *native);

    //==========================================================================
    // Scan Progress

    void encode(const ::platform::upgrade::ScanProgress &native,
                ::cc::platform::upgrade::ScanProgress *msg);

    void decode(const ::cc::platform::upgrade::ScanProgress &msg,
                ::platform::upgrade::ScanProgress *native);

    //==========================================================================
    // Upgrade Progress

    void encode(const ::platform::upgrade::UpgradeProgress &native,
                ::cc::platform::upgrade::UpgradeProgress *msg);

    void decode(const ::cc::platform::upgrade::UpgradeProgress &msg,
                ::platform::upgrade::UpgradeProgress *native);

    //==========================================================================
    // Upgrade Progress: State

    void encode(const ::platform::upgrade::UpgradeProgress::State &native,
                ::cc::platform::upgrade::UpgradeProgress::UpgradeState *msg);

    void decode(const ::cc::platform::upgrade::UpgradeProgress::UpgradeState &msg,
                ::platform::upgrade::UpgradeProgress::State *native);

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const ::platform::upgrade::UpgradeProgress::Fraction &native,
                ::cc::platform::upgrade::UpgradeProgress::ProgressFraction *msg);

    void decode(const ::cc::platform::upgrade::UpgradeProgress::ProgressFraction &msg,
                ::platform::upgrade::UpgradeProgress::Fraction *native);

}  // namespace protobuf
