// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-signalqueue.h++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "upgrade-grpc-signalqueue.h++"
#include "upgrade-signals.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace platform::upgrade::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::upgrade::Signal;

        this->connect<ScanProgress::Ref>(
            Signal::kScanProgress,
            platform::upgrade::signal_scan_progress,
            [=](ScanProgress::Ref ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_scan_progress());
            });

        this->connect<PackageInfo::Ref>(
            Signal::kUpgradeAvailable,
            platform::upgrade::signal_upgrade_available,
            [=](PackageInfo::Ref ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_upgrade_available());
            });

        this->connect<UpgradeProgress::Ref>(
            Signal::kUpgradeProgress,
            platform::upgrade::signal_upgrade_progress,
            [=](UpgradeProgress::Ref ref, Signal *msg) {
                msg->set_mapping_action(this->boolean_mapping(bool(ref)));
                protobuf::encode_shared(ref, msg->mutable_upgrade_progress());
            });

        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(platform::upgrade::signal_upgrade_progress);
        this->disconnect(platform::upgrade::signal_upgrade_available);
        this->disconnect(platform::upgrade::signal_scan_progress);
        Super::deinitialize();
    }
}  // namespace platform::upgrade::grpc
