// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.c++
/// @brief Upgrade implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-native-provider.h++"
#include "package-handler-vfs.h++"
#include "upgrade-settings.h++"

namespace platform::upgrade::native
{
    NativeProvider::NativeProvider()
        : Super("native"),
          settings(core::SettingsStore::create_shared(SETTINGS_FILE))
    {
    }

    void NativeProvider::initialize()
    {
        Super::initialize();
    }

    void NativeProvider::scan(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case PackageSource::LOC_NONE:
            if (core::types::Value default_url = this->settings->get(SETTING_DOWNLOAD_URL))
            {
                this->scan_http(default_url.as_string());
            }
            break;

        case PackageSource::LOC_VFS:
            this->scan_vfs(source.vfs_path().value());
            break;

        case PackageSource::LOC_URL:
            this->scan_http(source.url().value());
            break;
        }
    }

    std::vector<PackageInfo::Ref> NativeProvider::list_available() const
    {
        return {};
    }

    PackageInfo::Ref NativeProvider::best_available() const
    {
        return {};
    }

    PackageInfo::Ref NativeProvider::install(const PackageSource &source)
    {
        fs::path staging_folder = core::platform::path->mktemp();
        return {};
    }

    void NativeProvider::finalize()
    {
    }

    void NativeProvider::scan_vfs(const vfs::Path &source)
    {
        std::shared_ptr<VFSPackageHandler> handler = this->vfs_handlers[source];
        if (!handler)
        {
            handler = std::make_shared<VFSPackageHandler>(this->settings, source);
            this->vfs_handlers[source] = handler;
        }
        handler->scan();
    }

    void NativeProvider::scan_http(const URL &source)
    {
        std::shared_ptr<HTTPPackageHandler> handler = this->http_handlers[source];
        if (!handler)
        {
            handler = std::make_shared<HTTPPackageHandler>(this->settings, source);
            this->http_handlers[source] = handler;
        }
        handler->scan();
    }

    void NativeProvider::remove(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case PackageSource::LOC_VFS:
            this->remove_vfs(source.vfs_path().value());
            break;

        case PackageSource::LOC_URL:
            this->remove_http(source.url().value());
            break;
        }
    }

    void NativeProvider::remove_vfs(const vfs::Path &source)
    {
        this->vfs_handlers.erase(source);
    }

    void NativeProvider::remove_http(const URL &source)
    {
        this->http_handlers.erase(source);
    }

}  // namespace platform::upgrade::native
