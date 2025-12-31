// -*- c++ -*-
//==============================================================================
/// @file package-index.h++
/// @brief Abstract package index
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "upgrade-types.h++"

namespace upgrade::native
{
    class PackageIndex
    {
        using This = PackageIndex;

    public:
        using ptr = std::shared_ptr<This>;

    public:
        virtual PackageSource package_source(const std::string &package_name) const = 0;
        virtual PackageCatalogue scan() = 0;
        virtual std::vector<PackageInfo::ptr> get_available() const;
        virtual std::size_t get_available_count() const;

    protected:
        void emit_scan_progress(
            const std::optional<PackageSource> &source) const;

    protected:
        std::vector<PackageInfo::ptr> available_packages;
    };
}  // namespace upgrade::native
