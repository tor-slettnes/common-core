/// -*- c++ -*-
//==============================================================================
/// @file protobuf-upgrade-types.h++
/// @brief Encode/decode routines for upgrade ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "cc/platform/upgrade/protobuf/upgrade_types.pb.h"  // generated from `variant.proto`
#include "upgrade-base.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Package Source

    void encode(const platform::upgrade::PackageSource& native,
                platform::upgrade::protobuf::PackageSource* msg);

    void decode(const platform::upgrade::protobuf::PackageSource& msg,
                platform::upgrade::PackageSource* native);

    //==========================================================================
    // Package Sources

    void encode(const platform::upgrade::PackageSources& native,
                platform::upgrade::protobuf::PackageSources* msg);

    void decode(const platform::upgrade::protobuf::PackageSources& msg,
                platform::upgrade::PackageSources* native);

    //==========================================================================
    // Package Information

    void encode(const platform::upgrade::PackageInfo& native,
                platform::upgrade::protobuf::PackageInfo* msg);

    void decode(const platform::upgrade::protobuf::PackageInfo& msg,
                platform::upgrade::PackageInfo* native);

    //==========================================================================
    // Package Catalogue

    void encode(const platform::upgrade::PackageCatalogue& native,
                platform::upgrade::protobuf::PackageCatalogue* msg);

    void decode(const platform::upgrade::protobuf::PackageCatalogue& msg,
                platform::upgrade::PackageCatalogue* native);

    //==========================================================================
    // Scan Progress

    void encode(const platform::upgrade::ScanProgress& native,
                platform::upgrade::protobuf::ScanProgress* msg);

    void decode(const platform::upgrade::protobuf::ScanProgress& msg,
                platform::upgrade::ScanProgress* native);

    //==========================================================================
    // Upgrade Progress

    void encode(const platform::upgrade::UpgradeProgress& native,
                platform::upgrade::protobuf::UpgradeProgress* msg);

    void decode(const platform::upgrade::protobuf::UpgradeProgress& msg,
                platform::upgrade::UpgradeProgress* native);

    //==========================================================================
    // Upgrade Progress: State

    void encode(const platform::upgrade::UpgradeProgress::State& native,
                platform::upgrade::protobuf::UpgradeState* msg);

    void decode(const platform::upgrade::protobuf::UpgradeState& msg,
                platform::upgrade::UpgradeProgress::State* native);

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const platform::upgrade::UpgradeProgress::Fraction& native,
                platform::upgrade::protobuf::UpgradeProgress::ProgressFraction* msg);

    void decode(const platform::upgrade::protobuf::UpgradeProgress::ProgressFraction& msg,
                platform::upgrade::UpgradeProgress::Fraction* native);

}  // namespace cc::protobuf
