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
        switch (native.location_type())
        {
        case ::platform::upgrade::LocationType::VFS:
            encode(native.vfs_path(), msg->mutable_vfs_path());
            break;;

        case ::platform::upgrade::LocationType::URL:
            msg->set_url(native.url());
            break;
        }
    }

    void decode(const ::cc::platform::upgrade::PackageSource &msg,
                ::platform::upgrade::PackageSource *native)
    {
        switch (msg.location_case())
        {
        case ::cc::platform::upgrade::PackageSource::kVfsPath:
            native->location = decoded<::platform::vfs::Path>(msg.vfs_path());
            break;

        case ::cc::platform::upgrade::PackageSource::kUrl:
            native->location = msg.url();
            break;

        default:
            native->location = {};
            break;
        }
    }

    //==========================================================================
    // Package Sources

    void encode(const ::platform::upgrade::PackageSources &native,
                ::cc::platform::upgrade::PackageSources *msg)
    {
        auto sources = msg->mutable_sources();
        sources->Clear();
        sources->Reserve(native.size());
        for (const ::platform::upgrade::PackageSource &src: native)
        {
            encode(src, sources->Add());
        }
    }

    void decode(const ::cc::platform::upgrade::PackageSources &msg,
                ::platform::upgrade::PackageSources *native)
    {
        native->clear();
        native->reserve(msg.sources().size());
        for (const ::cc::platform::upgrade::PackageSource &src: msg.sources())
        {
            decode(src, &native->emplace_back());
        }
    }

    //==========================================================================
    // Package Manifest

    void encode(const ::platform::upgrade::PackageManifest &native,
                ::cc::platform::upgrade::PackageManifest *msg)
    {
        encode(native.source(), msg->mutable_source());
        msg->set_product_name(native.product());
        encode(native.version(), msg->mutable_release_version());
        msg->set_release_description(native.description());
        msg->set_reboot_required(native.reboot_required());
        msg->set_is_applicable(native.is_applicable());
    }

    void decode(const ::cc::platform::upgrade::PackageManifest &msg,
                ::platform::upgrade::PackageManifest *native)
    {
        *native = ::platform::upgrade::PackageManifest(
            decoded<::platform::upgrade::PackageSource>(msg.source()),
            msg.product_name(),
            decoded<::platform::sysconfig::Version>(msg.release_version()),
            msg.release_description(),
            msg.reboot_required(),
            msg.is_applicable());
    }

    //==========================================================================
    // Package Manifests

    void encode(const ::platform::upgrade::PackageManifests &native,
                ::cc::platform::upgrade::PackageManifests *msg)
    {
        auto manifests = msg->mutable_manifests();
        manifests->Clear();
        manifests->Reserve(native.size());
        for (const ::platform::upgrade::PackageManifest::ptr &ptr: native)
        {
            encode(*ptr, manifests->Add());
        }
    }

    void decode(const ::cc::platform::upgrade::PackageManifests &msg,
                ::platform::upgrade::PackageManifests *native)
    {
        native->clear();
        native->reserve(msg.manifests().size());
        for (const ::cc::platform::upgrade::PackageManifest &manifest: msg.manifests())
        {
            decode_shared(manifest, &native->emplace_back());
        }
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
