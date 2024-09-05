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

    PackageSource::PackageSource(
        const Location &location,
        const fs::path &filename)
        : location(location),
          filename(filename)
    {
    }

    PackageSource::LocationType
    PackageSource::location_type() const
    {
        return static_cast<LocationType>(this->location.index());
    }

    std::optional<vfs::Path>
    PackageSource::vfs_path() const
    {
        if (auto *vpath = std::get_if<vfs::Path>(&this->location))
        {
            return *vpath;
        }
        else
        {
            return {};
        }
    }

    std::optional<URL>
    PackageSource::url() const
    {
        if (auto *url = std::get_if<URL>(&this->location))
        {
            return *url;
        }
        else
        {
            return {};
        }
    }

    void
    PackageSource::to_stream(
        std::ostream &stream) const
    {
        core::types::PartsList parts;

        switch (this->location_type())
        {
        case LOC_VFS:
            parts.add("vpath", this->vfs_path().value(), true, "%r");
            break;

        case LOC_URL:
            parts.add("url", this->url().value(), true, "%r");
            break;
        }

        parts.add("filename", this->filename, !this->filename.empty());
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

    const PackageSource &
    PackageManifest::source() const
    {
        return this->source_;
    }

    const std::string &
    PackageManifest::product() const
    {
        return this->product_;
    }

    const Version &
    PackageManifest::version() const
    {
        return this->version_;
    }

    const std::string &
    PackageManifest::description() const
    {
        return this->description_;
    }

    const bool
    PackageManifest::reboot_required() const
    {
        return this->reboot_required_;
    }

    const bool
    PackageManifest::is_applicable() const
    {
        return this->is_applicable_;
    }

    void
    PackageManifest::to_stream(std::ostream &stream) const
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

    //==========================================================================
    // ScanProgress

    void
    ScanProgress::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "{source=%s}",
                          this->source);
    }

    //==========================================================================
    // UpgradeProgress

    void
    UpgradeProgress::to_stream(std::ostream &stream) const
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

    void
    UpgradeProgress::Fraction::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "%d of %d",
                          this->current,
                          this->total);
    }

    std::ostream &
    operator<<(
        std::ostream &stream,
        UpgradeProgress::State state)
    {
        return UpgradeProgress::state_names.to_stream(stream, state);
    }

    core::types::SymbolMap<UpgradeProgress::State> UpgradeProgress::state_names = {
        {STATE_NONE, "NONE"},
        {STATE_DOWNLOADING, "DOWNLOADING"},
        {STATE_VALIDATING, "VALIDATING"},
        {STATE_UNPACKING, "UNPACKING"},
        {STATE_INSTALLING, "INSTALLING"},
        {STATE_FINISHED, "FINISHED"},
    };
}  // namespace platform::upgrade
