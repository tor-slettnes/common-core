// -*- c++ -*-
//==============================================================================
/// @file package-handler-http.h++
/// @brief Upgrade from local package
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-handler.h++"
#include "types/create-shared.h++"

namespace platform::upgrade::native
{
    class HTTPPackageHandler : public PackageHandler,
                               public core::types::enable_create_shared<HTTPPackageHandler>
    {
        using This = HTTPPackageHandler;
        using Super = PackageHandler;

    public:
        HTTPPackageHandler(const core::SettingsStore::Ref &settings,
                           const URL &url);

    public:
        void scan() override;
        void unpack(const PackageInfo::Ref &info,
                    const fs::path &staging_folder) override;

    private:
        URL url;
    };
}  // namespace platform::upgrade::native
