/// -*- c++ -*-
//==============================================================================
/// @file protobuf-upgrade-types.h++
/// @brief Encode/decode routines for upgrade ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/upgrade/protobuf/upgrade_types.pb.h"  // generated from `variant.proto`
#include "upgrade-base.h++"

namespace protobuf
{
    //==========================================================================
    // Package Source

    void encode(const ::upgrade::PackageSource &native,
                ::cc::platform::upgrade::protobuf::PackageSource *msg);

    void decode(const ::cc::platform::upgrade::protobuf::PackageSource &msg,
                ::upgrade::PackageSource *native);

    //==========================================================================
    // Package Sources

    void encode(const ::upgrade::PackageSources &native,
                ::cc::platform::upgrade::protobuf::PackageSources *msg);

    void decode(const ::cc::platform::upgrade::protobuf::PackageSources &msg,
                ::upgrade::PackageSources *native);

    //==========================================================================
    // Package Information

    void encode(const ::upgrade::PackageInfo &native,
                ::cc::platform::upgrade::protobuf::PackageInfo *msg);

    void decode(const ::cc::platform::upgrade::protobuf::PackageInfo &msg,
                ::upgrade::PackageInfo *native);

    //==========================================================================
    // Package Catalogue

    void encode(const ::upgrade::PackageCatalogue &native,
                ::cc::platform::upgrade::protobuf::PackageCatalogue *msg);

    void decode(const ::cc::platform::upgrade::protobuf::PackageCatalogue &msg,
                ::upgrade::PackageCatalogue *native);

    //==========================================================================
    // Scan Progress

    void encode(const ::upgrade::ScanProgress &native,
                ::cc::platform::upgrade::protobuf::ScanProgress *msg);

    void decode(const ::cc::platform::upgrade::protobuf::ScanProgress &msg,
                ::upgrade::ScanProgress *native);

    //==========================================================================
    // Upgrade Progress

    void encode(const ::upgrade::UpgradeProgress &native,
                ::cc::platform::upgrade::protobuf::UpgradeProgress *msg);

    void decode(const ::cc::platform::upgrade::protobuf::UpgradeProgress &msg,
                ::upgrade::UpgradeProgress *native);

    //==========================================================================
    // Upgrade Progress: State

    void encode(const ::upgrade::UpgradeProgress::State &native,
                ::cc::platform::upgrade::protobuf::UpgradeState *msg);

    void decode(const ::cc::platform::upgrade::protobuf::UpgradeState &msg,
                ::upgrade::UpgradeProgress::State *native);

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const ::upgrade::UpgradeProgress::Fraction &native,
                ::cc::platform::upgrade::protobuf::UpgradeProgress::ProgressFraction *msg);

    void decode(const ::cc::platform::upgrade::protobuf::UpgradeProgress::ProgressFraction &msg,
                ::upgrade::UpgradeProgress::Fraction *native);

}  // namespace protobuf
