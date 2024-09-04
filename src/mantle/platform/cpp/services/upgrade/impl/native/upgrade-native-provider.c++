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
          settings(core::SettingsStore::create_shared(SETTINGS_FILE)),
          default_vfs_path(settings->get(SETTING_VFS_CONTEXT, DEFAULT_VFS_CONTEXT).as_string(), {}),
          default_url(settings->get(SETTING_DOWNLOAD_URL).as_string())
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
            if (!this->default_url.empty())
            {
                this->scan_http(this->default_url);
            }
            break;

        case PackageSource::LOC_VFS:
            this->scan_vfs(source.vfs_path().value_or(this->default_vfs_path));
            break;

        case PackageSource::LOC_URL:
            this->scan_http(source.url().value_or(this->default_url));
            break;
        }
    }

    PackageSources NativeProvider::list_sources() const
    {
        PackageSources sources;
        for (const std::shared_ptr<PackageHandler> &handler : this->handlers())
        {
            sources.push_back(handler->get_source());
        }
        return sources;
    }

    PackageManifests NativeProvider::list_available() const
    {
        PackageManifests available;
        for (const std::shared_ptr<PackageHandler> &handler : this->handlers())
        {
            for (const PackageManifest::ptr &pkg_info : handler->get_available())
            {
                available.push_back(pkg_info);
            }
        }
        return available;
    }

    PackageManifest::ptr NativeProvider::best_available() const
    {
        PackageManifest::ptr best;
        for (const PackageManifest::ptr &candidate : this->list_available())
        {
        }
        return best;
    }

    PackageManifest::ptr NativeProvider::install(const PackageSource &source)
    {
        fs::path staging_folder = core::platform::path->mktemp();
        return {};
    }

    void NativeProvider::finalize()
    {
    }

    void NativeProvider::scan_vfs(const vfs::Path &source)
    {
        this->vfs_handlers.emplace_shared(source, this->settings, source)
            ->scan();
    }

    void NativeProvider::scan_http(const URL &source)
    {
        this->http_handlers.emplace_shared(source, this->settings, source)
            ->scan();
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

    std::vector<std::shared_ptr<PackageHandler>> NativeProvider::handlers() const
    {
        std::vector<std::shared_ptr<PackageHandler>> handlers;
        handlers.reserve(this->vfs_handlers.size() + this->http_handlers.size());

        for (const auto &[vfs_path, handler] : this->vfs_handlers)
        {
            handlers.push_back(handler);
        }

        for (const auto &[url, handler] : this->http_handlers)
        {
            handlers.push_back(handler);
        }
        return handlers;
    }

}  // namespace platform::upgrade::native
