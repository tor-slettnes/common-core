// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-grpc-signalqueue.h++"
#include "upgrade-signals.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::upgrade::grpc
{
    void SignalQueue::initialize()
    {
        using protobuf::Signal;

        this->connect<ScanProgress::ptr>(
            protobuf::Signal::kScanProgress,
            signal_scan_progress,
            [=](ScanProgress::ptr ref, protobuf::Signal* msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                cc::protobuf::encode_shared(ref, msg->mutable_scan_progress());
            });

        this->connect<PackageInfo::ptr>(
            protobuf::Signal::kUpgradeAvailable,
            signal_upgrade_available,
            [=](PackageInfo::ptr ref, protobuf::Signal* msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                cc::protobuf::encode_shared(ref, msg->mutable_upgrade_available());
            });

        this->connect<PackageInfo::ptr>(
            protobuf::Signal::kUpgradePending,
            signal_upgrade_pending,
            [=](PackageInfo::ptr ref, protobuf::Signal* msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                cc::protobuf::encode_shared(ref, msg->mutable_upgrade_pending());
            });

        this->connect<UpgradeProgress::ptr>(
            protobuf::Signal::kUpgradeProgress,
            signal_upgrade_progress,
            [=](UpgradeProgress::ptr ref, protobuf::Signal* msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                cc::protobuf::encode_shared(ref, msg->mutable_upgrade_progress());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(signal_upgrade_progress);
        this->disconnect(signal_upgrade_pending);
        this->disconnect(signal_upgrade_available);
        this->disconnect(signal_scan_progress);
        Super::deinitialize();
    }
}  // namespace cc::platform::upgrade::grpc
