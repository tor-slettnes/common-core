// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-client.c++
/// @brief Upgrade gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-client.h++"
#include "upgrade.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace platform::upgrade::grpc
{
    void Client::initialize()
    {
        Super::initialize();

        this->add_handler(
            cc::platform::upgrade::Signal::kScanProgress,
            &This::on_scan_progress);

        this->add_handler(
            cc::platform::upgrade::Signal::kUpgradeAvailable,
            &This::on_upgrade_available);

        this->add_handler(
            cc::platform::upgrade::Signal::kUpgradePending,
            &This::on_upgrade_pending);

        this->add_handler(
            cc::platform::upgrade::Signal::kUpgradeProgress,
            &This::on_upgrade_progress);
    }

    void Client::on_scan_progress(const cc::platform::upgrade::Signal &signal)
    {
        platform::upgrade::ScanProgress::ptr progress;
        if (This::is_mapped(signal.mapping_action()))
        {
            protobuf::decode_shared(signal.scan_progress(), &progress);
        }
        platform::upgrade::signal_scan_progress.emit(progress);
    }

    void Client::on_upgrade_available(const cc::platform::upgrade::Signal &signal)
    {
        platform::upgrade::PackageManifest::ptr available_info;
        if (This::is_mapped(signal.mapping_action()))
        {
            protobuf::decode_shared(signal.upgrade_available(), &available_info);
        }
        platform::upgrade::signal_upgrade_available.emit(available_info);
    }

    void Client::on_upgrade_pending(const cc::platform::upgrade::Signal &signal)
    {
        platform::upgrade::PackageManifest::ptr pending_info;
        if (This::is_mapped(signal.mapping_action()))
        {
            protobuf::decode_shared(signal.upgrade_pending(), &pending_info);
        }
        platform::upgrade::signal_upgrade_pending.emit(pending_info);
    }

    void Client::on_upgrade_progress(const cc::platform::upgrade::Signal &signal)
    {
        platform::upgrade::UpgradeProgress::ptr progress;
        if (This::is_mapped(signal.mapping_action()))
        {
            protobuf::decode_shared(signal.upgrade_progress(), &progress);
        }
        platform::upgrade::signal_upgrade_progress.emit(progress);
    }

}  // namespace platform::upgrade::grpc
