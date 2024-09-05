// -*- c++ -*-
//==============================================================================
/// @file package-handler-http.c++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "package-handler-http.h++"

namespace platform::upgrade::native
{
    HTTPPackageHandler::HTTPPackageHandler(const core::SettingsStore::ptr &settings,
                                           const URL &url)
        : Super(settings),
          url(url)
    {
    }

    PackageSource HTTPPackageHandler::get_source() const
    {
        return {this->url};
    }

    void HTTPPackageHandler::scan()
    {
    }

    void HTTPPackageHandler::unpack(const fs::path &filename,
                                    const fs::path &staging_folder)
    {
    }
}  // namespace platform::upgrade::native
