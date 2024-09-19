// -*- c++ -*-
//==============================================================================
/// @file package-index-url.h++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-index.h++"
#include "types/create-shared.h++"

namespace platform::upgrade::native
{
    class URLPackageIndex : public PackageIndex
    {
        using This = URLPackageIndex;
        using Super = PackageIndex;

    public:
        URLPackageIndex(const URL &url);

        PackageSource package_source(const std::string &package_name) const override;
        PackageManifests scan() override;

    private:
        URL url;
    };
}  // namespace platform::upgrade::native
