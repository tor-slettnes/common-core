// -*- c++ -*-
//==============================================================================
/// @file upgrade-types.c++
/// @brief Upgrade service - data types
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-types.h++"

namespace upgrade
{
    //==========================================================================
    // PackageSource

    PackageSource::PackageSource(const Location &location)
        : location(location)
    {
    }

    PackageSource::operator bool() const noexcept
    {
        return this->location_type() != LocationType::NONE;
    }

    bool PackageSource::empty() const noexcept
    {
        return this->location_type() == LocationType::NONE;
    }

    LocationType PackageSource::location_type() const
    {
        return static_cast<LocationType>(this->location.index());
    }

    vfs::Path PackageSource::vfs_path(const vfs::Path &fallback) const
    {
        if (auto *candidate = std::get_if<vfs::Path>(&this->location))
        {
            if (*candidate)
            {
                return *candidate;
            }
        }
        return fallback;
    }

    URL PackageSource::url(const URL &fallback) const
    {
        if (auto *candidate = std::get_if<URL>(&this->location))
        {
            if (!candidate->empty())
            {
                return *candidate;
            }
        }
        return fallback;
    }

    fs::path PackageSource::filename() const
    {
        switch (this->location_type())
        {
        case LocationType::VFS:
            return this->vfs_path().filename();

        case LocationType::URL:
            return fs::path(this->url()).filename();

        default:
            return {};
        }
    }

    PackageSource PackageSource::remove_filename() const
    {
        switch (this->location_type())
        {
        case LocationType::VFS:
            return {this->vfs_path().remove_filename()};

        case LocationType::URL:
            return {fs::path(this->url()).remove_filename()};

        default:
            return {};
        }
    }

    void PackageSource::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        switch (this->location_type())
        {
        case LocationType::VFS:
            tvlist->append("vpath", this->vfs_path().as_tvlist());
            break;

        case LocationType::URL:
            tvlist->append("url", this->url());
            break;

        default:
            break;
        }
    }

    //==========================================================================
    // PackageInfo

    PackageInfo::PackageInfo(
        const PackageSource &source,
        const std::string &product,
        const sysconfig::Version &version,
        const std::string &description,
        bool reboot_required,
        bool is_applicable)
        : source_(source),
          product_(product),
          version_(version),
          description_(description),
          reboot_required_(reboot_required),
          is_applicable_(is_applicable)
    {
    }

    const PackageSource &PackageInfo::source() const
    {
        return this->source_;
    }

    const std::string &PackageInfo::product() const
    {
        return this->product_;
    }

    const Version &PackageInfo::version() const
    {
        return this->version_;
    }

    const std::string &PackageInfo::description() const
    {
        return this->description_;
    }

    bool PackageInfo::reboot_required() const
    {
        return this->reboot_required_;
    }

    bool PackageInfo::is_applicable() const
    {
        return this->is_applicable_;
    }

    bool PackageInfo::operator==(const PackageInfo &other) const
    {
        return (this->source() == other.source()) &&
               (this->product() == other.product()) &&
               (this->version() == other.version()) &&
               (this->description() == other.description()) &&
               (this->reboot_required() == other.reboot_required()) &&
               (this->is_applicable() == other.is_applicable());
    }

    void PackageInfo::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        if (this->source())
        {
            tvlist->extend({
                {"source", this->source().as_tvlist()},
                {"product", this->product()},
                {"version", this->version().as_tvlist()},
                {"description", this->description()},
                {"reboot_required", this->reboot_required()},
                {"is_applicable", this->is_applicable()},
            });
        }
    }

    //==========================================================================
    // ScanProgress

    void ScanProgress::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("source", this->source.as_tvlist());
    }

    //==========================================================================
    // UpgradeProgress

    void UpgradeProgress::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("state", this->state);
        tvlist->append("task_description", this->task_description);
        tvlist->append("task_progress", this->task_progress.as_tvlist());
        tvlist->append("total_progress", this->total_progress.as_tvlist());
        if (this->error)
        {
            tvlist->append("error", this->error->as_tvlist());
        }
    }

    UpgradeProgress::Fraction::Fraction(const core::types::Value &current,
                                        const core::types::Value &total)
        : current(current.as_uint32()),
          total(total.as_uint32(100))
    {
    }

    void UpgradeProgress::Fraction::to_tvlist(core::types::TaggedValueList *tvlist) const
    {
        tvlist->append("current", this->current);
        tvlist->append("total", this->total);
    }

    std::ostream &operator<<(std::ostream &stream, UpgradeProgress::State state)
    {
        return UpgradeProgress::state_names.to_stream(stream, state);
    }

    core::types::SymbolMap<UpgradeProgress::State> UpgradeProgress::state_names = {
        {STATE_NONE, "NONE"},
        {STATE_DOWNLOADING, "DOWNLOADING"},
        {STATE_UNPACKING, "UNPACKING"},
        {STATE_INSTALLING, "INSTALLING"},
        {STATE_COMPLETED, "COMPLETED"},
        {STATE_FAILED, "FAILED"},
        {STATE_FINALIZED, "FINALIZED"},
    };
}  // namespace upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const upgrade::Location &location)
    {
        if (auto *vpath = std::get_if<vfs::Path>(&location))
        {
            stream << "{vfs_path=" << *vpath << "}";
        }
        else if (auto *url = std::get_if<upgrade::URL>(&location))
        {
            stream << "{url=" << *url << "}";
        }
        else
        {
            stream << "{}";
        }
        return stream;
    }
}  // namespace std
