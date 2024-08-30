// -*- c++ -*-
//==============================================================================
/// @file package-handler-http.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler-http.h++"

namespace platform::upgrade::native
{
    HTTPPackageHandler::HTTPPackageHandler(const core::SettingsStore::Ref &settings,
                                           const URL &url)
        : Super(settings),
          url(url)
    {
    }

    void HTTPPackageHandler::scan()
    {
    }

    void HTTPPackageHandler::unpack(const PackageInfo::Ref &info,
                                    const fs::path &staging_folder)
    {
    }
}  // namespace platform::upgrade::native
