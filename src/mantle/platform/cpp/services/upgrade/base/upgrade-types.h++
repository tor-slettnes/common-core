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
#include "types/listable.h++"

namespace upgrade
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
        vfs::Path,
        URL>;

    //==========================================================================
    // PackageSource

    struct PackageSource : public core::types::Listable
    {
    public:
        PackageSource(const Location &location = {});

        operator bool() const noexcept;
        bool empty() const noexcept;

        LocationType location_type() const;
        vfs::Path vfs_path(const vfs::Path &fallback = {}) const;
        URL url(const URL &fallback = {}) const;
        fs::path filename() const;
        PackageSource remove_filename() const;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    public:
        Location location;
    };

    using PackageSources = std::vector<PackageSource>;

    //==========================================================================
    // PackageInfo

    struct PackageInfo : public core::types::Listable
    {
    public:
        using ptr = std::shared_ptr<PackageInfo>;

    public:
        PackageInfo(const PackageSource &source = {},
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
        virtual bool operator==(const PackageInfo &other) const;

    protected:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    private:
        PackageSource source_;
        std::string product_;
        Version version_;
        std::string description_;
        bool reboot_required_;
        bool is_applicable_;
    };

    using PackageCatalogue = std::vector<PackageInfo::ptr>;

    //==========================================================================
    // ScanProgress

    struct ScanProgress : public core::types::Listable
    {
        using ptr = std::shared_ptr<ScanProgress>;

    public:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    public:
        PackageSource source;
    };

    //==========================================================================
    // upgradeProgress

    struct UpgradeProgress : public core::types::Listable
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

        class Fraction : public core::types::Listable
        {
        public:
            Fraction(const core::types::Value &current = 0,
                     const core::types::Value &total = 0);

            void to_tvlist(core::types::TaggedValueList *tvlist) const override;

        public:
            std::uint32_t current = 0;
            std::uint32_t total = 0;
        };

    public:
        void to_tvlist(core::types::TaggedValueList *tvlist) const override;

    public:
        static core::types::SymbolMap<State> state_names;

        State state = STATE_NONE;
        std::string task_description;
        Fraction task_progress;
        Fraction total_progress;
        core::status::Error::ptr error;
    };

    static std::ostream &operator<<(std::ostream &stream,
                                    UpgradeProgress::State state);

}  // namespace upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const upgrade::Location &location);
}
