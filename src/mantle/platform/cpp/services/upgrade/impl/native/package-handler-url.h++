// -*- c++ -*-
//==============================================================================
/// @file package-handler-url.h++
/// @brief Upgrade from a HTTP source
/// @author Tor Slettnes
//==============================================================================

#pragma once
#include "package-handler.h++"
#include "types/create-shared.h++"

#include "http-client.h++"

namespace upgrade::native
{
    class URLPackageHandler : public PackageHandler,
                              public core::types::enable_create_shared<URLPackageHandler>
    {
        using This = URLPackageHandler;
        using Super = PackageHandler;

    public:
        URLPackageHandler(const core::SettingsStore::ptr &settings);

    public:
        void unpack(
            const PackageSource &source,
            const fs::path &staging_folder) override;

    private:
        void unpack_url(
            const URL &url,
            const fs::path &staging_folder) const;

        static core::http::HTTPClient::ReceiveFunction piper(
            core::platform::FileDescriptor fd);
    };
}  // namespace upgrade::native
