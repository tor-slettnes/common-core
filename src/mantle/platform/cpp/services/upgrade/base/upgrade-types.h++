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

    enum class LocationType
    {
        NONE,
        VFS,
        URL
    };

    using Location = std::variant<
        std::monostate,
        platform::vfs::Path,
        URL>;

    //==========================================================================
    // PackageSource

    struct PackageSource : public core::types::Streamable
    {
    public:
        PackageSource(const Location &location = {});

        operator bool() const noexcept;
        bool empty() const noexcept;

        LocationType location_type() const;
        vfs::Path vfs_path(const vfs::Path &fallback = {}) const;
        URL url(const URL &fallback = {}) const;
        fs::path filename() const;
        PackageSource parent_path() const;

    protected:
        void to_stream(std::ostream &stream) const override;

    public:
        Location location;
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
        virtual bool reboot_required() const;
        virtual bool is_applicable() const;

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
            STATE_UNPACKING,
            STATE_INSTALLING,
            STATE_COMPLETED,
            STATE_FAILED,
            STATE_FINALIZED = 9,
        };

        class Fraction : public core::types::Streamable
        {
        public:
            Fraction(const core::types::Value &current = 0,
                     const core::types::Value &total = 0);

            void to_stream(std::ostream &stream) const override;

        public:
            std::uint32_t current = 0;
            std::uint32_t total = 0;
        };

    public:
        void to_stream(std::ostream &stream) const override;

    public:
        static core::types::SymbolMap<State> state_names;

        State state = STATE_NONE;
        std::string task_description;
        Fraction task_progress;
        Fraction total_progress;
        core::status::Event::ptr error;
    };

    static std::ostream &operator<<(std::ostream &stream,
                                    UpgradeProgress::State state);

}  // namespace platform::upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const platform::upgrade::Location &location);
}
