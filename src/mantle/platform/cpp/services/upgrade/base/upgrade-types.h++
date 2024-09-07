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

        operator bool() const noexcept;

        LocationType location_type() const;
        vfs::Path vfs_path(const vfs::Path &fallback={}) const;
        URL url(const URL &fallback={}) const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        Location location;
        fs::path filename;
    };

    using PackageSources = std::vector<PackageSource>;

    //==========================================================================
    // PackageManifest

    struct PackageManifest : public core::types::Streamable
    {
    public:
        using ptr = std::shared_ptr<PackageManifest>;

    public:
        PackageManifest(const PackageSource &source = {},
                        const std::string &product = {},
                        const sysconfig::Version &version = {},
                        const std::string &description = {},
                        bool reboot_required = false,
                        bool applicable = false);

    public:
        virtual const PackageSource &source() const;
        virtual const std::string &product() const;
        virtual const Version &version() const;
        virtual const std::string &description() const;
        virtual const bool reboot_required() const;
        virtual const bool is_applicable() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    private:
        PackageSource source_;
        std::string product_;
        Version version_;
        std::string description_;
        bool reboot_required_;
        bool is_applicable_;
    };

    using PackageManifests = std::vector<PackageManifest::ptr>;

    //==========================================================================
    // ScanProgress

    struct ScanProgress : public core::types::Streamable
    {
        using ptr = std::shared_ptr<ScanProgress>;

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        PackageSource source;
    };

    //==========================================================================
    // upgradeProgress

    struct UpgradeProgress : public core::types::Streamable
    {
        using ptr = std::shared_ptr<UpgradeProgress>;

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
        core::status::Event::ptr error;
    };

    static std::ostream &operator<<(std::ostream &stream,
                                    UpgradeProgress::State state);

}  // namespace platform::upgrade
