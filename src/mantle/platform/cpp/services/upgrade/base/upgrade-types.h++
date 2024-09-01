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
#include "types/symbolmap.h++"

namespace platform::upgrade
{
    define_log_scope("upgrade");

    using sysconfig::Version;
    using URL = std::string;

    //==========================================================================
    // PackageSource

    struct PackageSource : public core::types::Streamable
    {
    public:
        enum LocationType
        {
            LOC_NONE,
            LOC_VFS,
            LOC_URL
        };

        using Location = std::variant<
            std::monostate,
            platform::vfs::Path,
            URL>;

    public:
        PackageSource(const Location &location = {},
                      const fs::path &filename = {});

        LocationType location_type() const;
        std::optional<vfs::Path> vfs_path() const;
        std::optional<URL> url() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        Location location;
        fs::path filename;
    };

    //==========================================================================
    // PackageInfo

    struct PackageInfo : public core::types::Streamable
    {
        using Ref = std::shared_ptr<PackageInfo>;

    public:
        PackageInfo(const PackageSource &source = {},
                    const std::string &product_name = {},
                    const sysconfig::Version &release_version = {},
                    const std::string &release_description = {},
                    bool reboot_Required = false,
                    bool applicable = false);

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        PackageSource source;
        std::string product_name;
        sysconfig::Version release_version;
        std::string release_description;
        bool reboot_required;
        bool applicable;
    };

    //==========================================================================
    // ScanProgress

    struct ScanProgress : public core::types::Streamable
    {
        using Ref = std::shared_ptr<ScanProgress>;

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        PackageSource source;
    };

    //==========================================================================
    // upgradeProgress

    struct UpgradeProgress : public core::types::Streamable
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

        struct Fraction : public core::types::Streamable
        {
        public:
            void to_stream(std::ostream &stream) const override;

        public:
            std::uint32_t current = 0;
            std::uint32_t total = 0;
        };

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        static core::types::SymbolMap<State> state_names;

        State state;
        std::string task_description;
        Fraction task_progress;
        Fraction total_progress;
        core::status::Event::Ref error;
    };

    static std::ostream &operator<<(std::ostream &stream,
                                    UpgradeProgress::State state);

}  // namespace platform::upgrade
