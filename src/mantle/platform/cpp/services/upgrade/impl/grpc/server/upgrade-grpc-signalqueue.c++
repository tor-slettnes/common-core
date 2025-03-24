// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-signalqueue.h++"
#include "upgrade-signals.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace upgrade::grpc
{
    void SignalQueue::initialize()
    {
        using ::cc::upgrade::Signal;

        this->connect<ScanProgress::ptr>(
            Signal::kScanProgress,
            upgrade::signal_scan_progress,
            [=](ScanProgress::ptr ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_scan_progress());
            });

        this->connect<PackageInfo::ptr>(
            Signal::kUpgradeAvailable,
            upgrade::signal_upgrade_available,
            [=](PackageInfo::ptr ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_upgrade_available());
            });

        this->connect<PackageInfo::ptr>(
            Signal::kUpgradePending,
            upgrade::signal_upgrade_pending,
            [=](PackageInfo::ptr ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_upgrade_pending());
            });

        this->connect<UpgradeProgress::ptr>(
            Signal::kUpgradeProgress,
            upgrade::signal_upgrade_progress,
            [=](UpgradeProgress::ptr ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_upgrade_progress());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(upgrade::signal_upgrade_progress);
        this->disconnect(upgrade::signal_upgrade_pending);
        this->disconnect(upgrade::signal_upgrade_available);
        this->disconnect(upgrade::signal_scan_progress);
        Super::deinitialize();
    }
}  // namespace upgrade::grpc
