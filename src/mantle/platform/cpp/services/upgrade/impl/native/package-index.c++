// -*- c++ -*-
//==============================================================================
/// @file package-index.c++
/// @brief Abstract package index
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-index.h++"
#include "upgrade-signals.h++"

namespace platform::upgrade::native
{
    std::vector<PackageInfo::ptr> PackageIndex::get_available() const
    {
        return this->available_packages;
    }

    std::size_t PackageIndex::get_available_count() const
    {
        return this->available_packages.size();
    }

    void PackageIndex::emit_scan_progress(
        const std::optional<PackageSource> &source) const
    {
        auto progress = std::make_shared<ScanProgress>();
        if (source)
        {
            progress->source = source.value();
        }
        signal_scan_progress.emit(progress);
    }
}  // namespace platform::upgrade::native
