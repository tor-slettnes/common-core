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

    PackageSource::PackageSource(const Location &location,
                                 const fs::path &filename)
        : location(location),
          filename(filename)
    {
    }

    PackageSource::LocationType PackageSource::location_type() const
    {
        return static_cast<LocationType>(this->location.index());
    }

    std::optional<vfs::Path> PackageSource::vfs_path() const
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

    std::optional<URL> PackageSource::url() const
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

    void PackageSource::to_stream(std::ostream &stream) const
    {
        core::types::PartsList parts;

        switch (this->location_type())
        {
        case LOC_VFS:
            parts.add("vpath", std::get<vfs::Path>(this->location));
            break;

        case LOC_URL:
            parts.add("url", std::get<URL>(this->location));
            break;
        }

        parts.add("filename", this->filename, !this->filename.empty());
        stream << parts;
    }

    //==========================================================================
    // PackageInfo

    PackageInfo::PackageInfo(const PackageSource &source,
                             const std::string &product_name,
                             const sysconfig::Version &release_version,
                             const std::string &release_description,
                             bool reboot_required,
                             bool applicable)
        : source(source),
          product_name(product_name),
          release_version(release_version),
          release_description(release_description),
          reboot_required(reboot_required),
          applicable(applicable)
    {
    }

    void PackageInfo::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "{source=%s, product_name=%r, "
                          "release_version=%s, release_description=%r, "
                          "reboot_required=%b, applicable=%b}",
                          this->source,
                          this->product_name,
                          this->release_version,
                          this->release_description,
                          this->reboot_required,
                          this->applicable);
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

    void UpgradeProgress::Fraction::to_stream(std::ostream &stream) const
    {
        core::str::format(stream,
                          "%d of %d",
                          this->current,
                          this->total);
    }

    std::ostream &operator<<(
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
