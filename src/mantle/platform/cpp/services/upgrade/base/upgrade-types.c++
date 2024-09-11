// -*- c++ -*-
//==============================================================================
/// @file upgrade-types.c++
/// @brief Upgrade service - data types
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-types.h++"

namespace platform::upgrade
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

    PackageSource PackageSource::parent_path() const
    {
        switch (this->location_type())
        {
        case LocationType::VFS:
            return {this->vfs_path().parent()};

        case LocationType::URL:
            return {fs::path(this->url()).parent_path()};

        default:
            return {};
        }
    }

    void PackageSource::to_stream(
        std::ostream &stream) const
    {
        core::types::PartsList parts;

        switch (this->location_type())
        {
        case LocationType::VFS:
            parts.add("vpath", this->vfs_path(), true, "%r");
            break;

        case LocationType::URL:
            parts.add("url", this->url(), true, "%r");
            break;
        }

        stream << parts;
    }

    //==========================================================================
    // PackageManifest

    PackageManifest::PackageManifest(
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

    const PackageSource &PackageManifest::source() const
    {
        return this->source_;
    }

    const std::string &PackageManifest::product() const
    {
        return this->product_;
    }

    const Version &PackageManifest::version() const
    {
        return this->version_;
    }

    const std::string &PackageManifest::description() const
    {
        return this->description_;
    }

    bool PackageManifest::reboot_required() const
    {
        return this->reboot_required_;
    }

    bool PackageManifest::is_applicable() const
    {
        return this->is_applicable_;
    }

    void PackageManifest::to_stream(std::ostream &stream) const
    {
        if (this->source())
        {
            core::str::format(stream,
                              "{source=%s, product=%r, "
                              "version=%r, description=%r, "
                              "reboot_required=%b, is_applicable=%b}",
                              this->source(),
                              this->product(),
                              this->version(),
                              this->description(),
                              this->reboot_required(),
                              this->is_applicable());
        }
        else
        {
            stream << "{}";
        }
    }

    //==========================================================================
    // ScanProgress

    void ScanProgress::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "{source=%s}",
                          this->source);
    }

    //==========================================================================
    // UpgradeProgress

    void UpgradeProgress::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;
        parts.add("state", this->state);
        parts.add_string("task_description", this->task_description);
        parts.add("task_progress", this->task_progress);
        parts.add("total_progress", this->total_progress);
        if (this->error)
        {
            parts.add("error", this->error);
        }
        stream << parts;
    }

    UpgradeProgress::Fraction::Fraction(const core::types::Value &current,
                                        const core::types::Value &total)
        : current(current.as_uint32()),
          total(total.as_uint32(100))
    {
    }

    void UpgradeProgress::Fraction::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "%d/%d",
                          this->current,
                          this->total);
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
}  // namespace platform::upgrade

namespace std
{
    std::ostream &operator<<(std::ostream &stream,
                             const platform::upgrade::Location &location)
    {
        if (auto *vpath = std::get_if<platform::vfs::Path>(&location))
        {
            stream << "{vfs_path=" << *vpath << "}";
        }
        else if (auto *url = std::get_if<platform::upgrade::URL>(&location))
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
