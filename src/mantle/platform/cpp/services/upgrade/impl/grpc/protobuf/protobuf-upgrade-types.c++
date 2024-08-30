/// -*- c++ -*-
//==============================================================================
/// @file protobuf-upgrade-types.c++
/// @brief Encode/decode routines for upgrade ProtoBuf types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "protobuf-upgrade-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace protobuf
{
    //==========================================================================
    // Package Source

    void encode(const ::platform::upgrade::PackageSource &native,
                ::cc::platform::upgrade::PackageSource *msg)
    {
        if (auto *vfs_path = std::get_if<platform::vfs::Path>(&native))
        {
            encode(*vfs_path, msg->mutable_vfs_path());
        }
        else if (auto *url = std::get_if<platform::upgrade::URL>(&native))
        {
            msg->set_url(*url);
        }
    }

    void decode(const ::cc::platform::upgrade::PackageSource &msg,
                ::platform::upgrade::PackageSource *native)
    {
        switch (msg.source_case())
        {
        case ::cc::platform::upgrade::PackageSource::kVfsPath:
            *native = decoded<::platform::vfs::Path>(msg.vfs_path());
            break;

        case ::cc::platform::upgrade::PackageSource::kUrl:
            *native = msg.url();
            break;

        default:
            *native = {};
            break;
        }
    }

    //==========================================================================
    // Package Info

    void encode(const ::platform::upgrade::PackageInfo &native,
                ::cc::platform::upgrade::PackageInfo *msg)
    {
        encode(native.source, msg->mutable_source());
        msg->set_package_name(native.package_name);
        msg->set_product_name(native.product_name);
        encode(native.release_version, msg->mutable_release_version());
        msg->set_release_description(native.release_description);
        msg->set_reboot_required(native.reboot_required);
        msg->set_applicable(native.applicable);
    }

    void decode(const ::cc::platform::upgrade::PackageInfo &msg,
                ::platform::upgrade::PackageInfo *native)
    {
        decode(msg.source(), &native->source);
        native->package_name = msg.package_name();
        native->product_name = msg.product_name();
        decode(msg.release_version(), &native->release_version);
        native->release_description = msg.release_description();
        native->reboot_required = msg.reboot_required();
        native->applicable = msg.applicable();
    }

    //==========================================================================
    // Scan Progress

    void encode(const ::platform::upgrade::ScanProgress &native,
                ::cc::platform::upgrade::ScanProgress *msg)
    {
        encode(native.source, msg->mutable_source());
    }

    void decode(const ::cc::platform::upgrade::ScanProgress &msg,
                ::platform::upgrade::ScanProgress *native)
    {
        decode(msg.source(), &native->source);
    }

    //==========================================================================
    // Upgrade Progress

    void encode(const ::platform::upgrade::UpgradeProgress &native,
                ::cc::platform::upgrade::UpgradeProgress *msg)
    {
        msg->set_state(encoded<cc::platform::upgrade::UpgradeProgress::UpgradeState>(native.state));
        msg->set_task_description(native.task_description);
        encode(native.task_progress, msg->mutable_task_progress());
        encode(native.total_progress, msg->mutable_total_progress());
        if (native.error)
        {
            encode_shared(native.error, msg->mutable_error());
        }
    }

    void decode(const ::cc::platform::upgrade::UpgradeProgress &msg,
                ::platform::upgrade::UpgradeProgress *native)
    {
        decode(msg.state(), &native->state);
        native->task_description = msg.task_description();
        decode(msg.task_progress(), &native->task_progress);
        decode(msg.total_progress(), &native->total_progress);
        if (msg.has_error())
        {
            decode_shared(msg.error(), &native->error);
        }
    }

    //==========================================================================
    // Upgrade Progress: State

    void encode(const ::platform::upgrade::UpgradeProgress::State &native,
                ::cc::platform::upgrade::UpgradeProgress::UpgradeState *msg)
    {
        *msg = static_cast<::cc::platform::upgrade::UpgradeProgress::UpgradeState>(native);
    }

    void decode(const ::cc::platform::upgrade::UpgradeProgress::UpgradeState &msg,
                ::platform::upgrade::UpgradeProgress::State *native)
    {
        *native = static_cast<::platform::upgrade::UpgradeProgress::State>(msg);
    }

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const ::platform::upgrade::UpgradeProgress::Fraction &native,
                ::cc::platform::upgrade::UpgradeProgress::ProgressFraction *msg)
    {
        msg->set_current(native.current);
        msg->set_total(native.total);
    }

    void decode(const ::cc::platform::upgrade::UpgradeProgress::ProgressFraction &msg,
                ::platform::upgrade::UpgradeProgress::Fraction *native)
    {
        native->current = msg.current();
        native->total = msg.total();
    }

}  // namespace protobuf
