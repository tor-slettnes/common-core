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

    void encode(const ::upgrade::PackageSource &native,
                ::cc::upgrade::PackageSource *msg);

    void decode(const ::cc::upgrade::PackageSource &msg,
                ::upgrade::PackageSource *native);

    //==========================================================================
    // Package Sources

    void encode(const ::upgrade::PackageSources &native,
                ::cc::upgrade::PackageSources *msg);

    void decode(const ::cc::upgrade::PackageSources &msg,
                ::upgrade::PackageSources *native);

    //==========================================================================
    // Package Information

    void encode(const ::upgrade::PackageInfo &native,
                ::cc::upgrade::PackageInfo *msg);

    void decode(const ::cc::upgrade::PackageInfo &msg,
                ::upgrade::PackageInfo *native);

    //==========================================================================
    // Package Catalogue

    void encode(const ::upgrade::PackageCatalogue &native,
                ::cc::upgrade::PackageCatalogue *msg);

    void decode(const ::cc::upgrade::PackageCatalogue &msg,
                ::upgrade::PackageCatalogue *native);

    //==========================================================================
    // Scan Progress

    void encode(const ::upgrade::ScanProgress &native,
                ::cc::upgrade::ScanProgress *msg);

    void decode(const ::cc::upgrade::ScanProgress &msg,
                ::upgrade::ScanProgress *native);

    //==========================================================================
    // Upgrade Progress

    void encode(const ::upgrade::UpgradeProgress &native,
                ::cc::upgrade::UpgradeProgress *msg);

    void decode(const ::cc::upgrade::UpgradeProgress &msg,
                ::upgrade::UpgradeProgress *native);

    //==========================================================================
    // Upgrade Progress: State

    void encode(const ::upgrade::UpgradeProgress::State &native,
                ::cc::upgrade::UpgradeProgress::UpgradeState *msg);

    void decode(const ::cc::upgrade::UpgradeProgress::UpgradeState &msg,
                ::upgrade::UpgradeProgress::State *native);

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const ::upgrade::UpgradeProgress::Fraction &native,
                ::cc::upgrade::UpgradeProgress::ProgressFraction *msg);

    void decode(const ::cc::upgrade::UpgradeProgress::ProgressFraction &msg,
                ::upgrade::UpgradeProgress::Fraction *native);

}  // namespace protobuf
