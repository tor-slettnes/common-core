/// -*- c++ -*-
//==============================================================================
/// @file protobuf-upgrade-types.c++
/// @brief Encode/decode routines for upgrade ProtoBuf types
/// @author Tor Slettnes
//==============================================================================

#include "protobuf-upgrade-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-vfs-types.h++"
#include "protobuf-event-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace cc::protobuf
{
    //==========================================================================
    // Package Source

    void encode(const platform::upgrade::PackageSource &native,
                platform::upgrade::protobuf::PackageSource *msg)
    {
        switch (native.location_type())
        {
        case platform::upgrade::LocationType::VFS:
            encode(native.vfs_path(), msg->mutable_vfs_path());
            break;

        case platform::upgrade::LocationType::URL:
            msg->set_url(native.url());
            break;

        default:
            break;
        }
    }

    void decode(const platform::upgrade::protobuf::PackageSource &msg,
                platform::upgrade::PackageSource *native)
    {
        switch (msg.location_case())
        {
        case platform::upgrade::protobuf::PackageSource::kVfsPath:
            native->location = decoded<platform::vfs::Path>(msg.vfs_path());
            break;

        case platform::upgrade::protobuf::PackageSource::kUrl:
            native->location = msg.url();
            break;

        default:
            native->location = {};
            break;
        }
    }

    //==========================================================================
    // Package Sources

    void encode(const platform::upgrade::PackageSources &native,
                platform::upgrade::protobuf::PackageSources *msg)
    {
        auto sources = msg->mutable_sources();
        sources->Clear();
        sources->Reserve(native.size());
        for (const platform::upgrade::PackageSource &src : native)
        {
            encode(src, sources->Add());
        }
    }

    void decode(const platform::upgrade::protobuf::PackageSources &msg,
                platform::upgrade::PackageSources *native)
    {
        native->clear();
        native->reserve(msg.sources().size());
        for (const platform::upgrade::protobuf::PackageSource &src : msg.sources())
        {
            decode(src, &native->emplace_back());
        }
    }

    //==========================================================================
    // Package Information

    void encode(const platform::upgrade::PackageInfo &native,
                platform::upgrade::protobuf::PackageInfo *msg)
    {
        encode(native.source(), msg->mutable_source());
        msg->set_product_name(native.product());
        encode(native.version(), msg->mutable_release_version());
        msg->set_release_description(native.description());
        msg->set_reboot_required(native.reboot_required());
        msg->set_is_applicable(native.is_applicable());
    }

    void decode(const platform::upgrade::protobuf::PackageInfo &msg,
                platform::upgrade::PackageInfo *native)
    {
        *native = platform::upgrade::PackageInfo(
            decoded<platform::upgrade::PackageSource>(msg.source()),
            msg.product_name(),
            decoded<platform::sysconfig::Version>(msg.release_version()),
            msg.release_description(),
            msg.reboot_required(),
            msg.is_applicable());
    }

    //==========================================================================
    // Package Catalogue

    void encode(const platform::upgrade::PackageCatalogue &native,
                platform::upgrade::protobuf::PackageCatalogue *msg)
    {
        auto packages = msg->mutable_packages();
        packages->Clear();
        packages->Reserve(native.size());
        for (const platform::upgrade::PackageInfo::ptr &ptr : native)
        {
            encode(*ptr, packages->Add());
        }
    }

    void decode(const platform::upgrade::protobuf::PackageCatalogue &msg,
                platform::upgrade::PackageCatalogue *native)
    {
        native->clear();
        native->reserve(msg.packages().size());
        for (const platform::upgrade::protobuf::PackageInfo &package_info : msg.packages())
        {
            decode_shared(package_info, &native->emplace_back());
        }
    }

    //==========================================================================
    // Scan Progress

    void encode(const platform::upgrade::ScanProgress &native,
                platform::upgrade::protobuf::ScanProgress *msg)
    {
        encode(native.source, msg->mutable_source());
    }

    void decode(const platform::upgrade::protobuf::ScanProgress &msg,
                platform::upgrade::ScanProgress *native)
    {
        decode(msg.source(), &native->source);
    }

    //==========================================================================
    // Upgrade Progress

    void encode(const platform::upgrade::UpgradeProgress &native,
                platform::upgrade::protobuf::UpgradeProgress *msg)
    {
        msg->set_state(encoded<platform::upgrade::protobuf::UpgradeState>(native.state));
        msg->set_task_description(native.task_description);
        encode(native.task_progress, msg->mutable_task_progress());
        encode(native.total_progress, msg->mutable_total_progress());
        if (native.error)
        {
            encode_shared(native.error, msg->mutable_error());
        }
    }

    void decode(const platform::upgrade::protobuf::UpgradeProgress &msg,
                platform::upgrade::UpgradeProgress *native)
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

    void encode(const platform::upgrade::UpgradeProgress::State &native,
                platform::upgrade::protobuf::UpgradeState *msg)
    {
        *msg = static_cast<platform::upgrade::protobuf::UpgradeState>(native);
    }

    void decode(const platform::upgrade::protobuf::UpgradeState &msg,
                platform::upgrade::UpgradeProgress::State *native)
    {
        *native = static_cast<platform::upgrade::UpgradeProgress::State>(msg);
    }

    //==========================================================================
    // Upgrade Progress: Fraction

    void encode(const platform::upgrade::UpgradeProgress::Fraction &native,
                platform::upgrade::protobuf::UpgradeProgress::ProgressFraction *msg)
    {
        msg->set_current(native.current);
        msg->set_total(native.total);
    }

    void decode(const platform::upgrade::protobuf::UpgradeProgress::ProgressFraction &msg,
                platform::upgrade::UpgradeProgress::Fraction *native)
    {
        native->current = msg.current();
        native->total = msg.total();
    }

}  // namespace cc::protobuf
