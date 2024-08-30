// -*- c++ -*-
//==============================================================================
/// @file upgrade-types.h++
/// @brief Upgrade service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "logging/logging.h++"
#include "sysconfig-product.h++"
#include "vfs-context.h++"
#include "status/event.h++"

namespace platform::upgrade
{
    define_log_scope("upgrade");

    enum PackageSourceType
    {
        SOURCE_NONE,
        SOURCE_VFS,
        SOURCE_URL
    };

    using sysconfig::Version;
    using URL = std::string;
    using PackageSource = std::variant<
        std::monostate,
        platform::vfs::Path,
        URL>;

    struct PackageInfo : public core::types::Streamable
    {
        using Ref = std::shared_ptr<PackageInfo>;

    public:
        PackageInfo(const PackageSource &source = {},
                    const fs::path &package_name = {},
                    const std::string &product_name = {},
                    const sysconfig::Version &release_version = {},
                    const std::string &release_description = {},
                    bool reboot_Required = false,
                    bool applicable = false);

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        PackageSource source;
        fs::path package_name;
        std::string product_name;
        sysconfig::Version release_version;
        std::string release_description;
        bool reboot_required;
        bool applicable;
    };

    struct ScanProgress
    {
        using Ref = std::shared_ptr<ScanProgress>;

        PackageSource source;
    };

    struct UpgradeProgress
    {
        using Ref = std::shared_ptr<UpgradeProgress>;

        enum State
        {
            STATE_NONE,
            STATE_DOWNLOADING,
            STATE_VALIDATING,
            STATE_UNPACKING,
            STATE_INSTALLING,
            STATE_FINISHED,
        };

        struct Fraction
        {
            std::uint32_t current = 0;
            std::uint32_t total = 0;
        };

        State state;
        std::string task_description;
        Fraction task_progress;
        Fraction total_progress;
        core::status::Event::Ref error;
    };
}  // namespace platform::upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const platform::upgrade::PackageSource &source);
}
