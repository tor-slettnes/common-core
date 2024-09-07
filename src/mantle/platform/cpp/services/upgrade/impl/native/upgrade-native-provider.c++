// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.c++
/// @brief Upgrade implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-native-provider.h++"
#include "package-handler-vfs.h++"
#include "upgrade-settings.h++"
#include "sysconfig-host.h++"
#include "status/exceptions.h++"

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
        if (source)
        {
            this->get_or_add_handler(source)->scan();
        }
        else
        {
            this->get_or_add_handler({this->default_vfs_path})->scan();
            if (!this->default_url.empty())
            {
                this->get_or_add_handler({this->default_url})->scan();
            }
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

    PackageManifests NativeProvider::list_available(const PackageSource &source) const
    {
        PackageManifests available;
        for (const std::shared_ptr<PackageHandler> &handler : this->handlers())
        {
            logf_debug("Considering package source %s", handler->get_source());

            for (const PackageManifest::ptr &pkg_info : handler->get_available())
            {
                available.push_back(pkg_info);
            }
        }
        return available;
    }

    PackageManifest::ptr NativeProvider::best_available(const PackageSource &source) const
    {
        PackageManifest::ptr best;
        for (const PackageManifest::ptr &candidate : this->list_available(source))
        {
            if (candidate->is_applicable() &&
                (!best || (candidate->version() > best->version())))
            {
                best = candidate;
            }
        }
        return best;
    }

    PackageManifest::ptr NativeProvider::install(const PackageSource &source)
    {
        PackageSource install_source;

        if (source && !source.filename.empty())
        {
            install_source = source;
        }
        else if (PackageManifest::ptr manifest = this->best_available(source))
        {
            install_source = manifest->source();
        }

        if (!this->install_lock.try_lock())
        {
            throw core::exception::Unavailable("An install task is already pending");
        }

        try
        {
            if (const PackageHandler::ptr &handler = this->get_or_add_handler(install_source))
            {
                return this->installed_manifest = handler->install(install_source);
            }
            else
            {
                throw core::exception::NotFound(
                    "No package file specified and "
                    "no applicable package discovered from prior scans");
            }
        }
        catch (...)
        {
            this->install_lock.unlock();
            throw;
        }
    }

    void NativeProvider::finalize()
    {
        if (this->install_lock.locked())
        {
            logf_info("Finalizing upgrade");
            if (this->installed_manifest && this->installed_manifest->reboot_required())
            {
                logf_notice("Rebooting system");
                platform::sysconfig::host->reboot();
            }

            this->install_lock.unlock();
        }
    }

    void NativeProvider::remove(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case PackageSource::LOC_VFS:
            this->vfs_handlers.erase(source.vfs_path());
            break;

        case PackageSource::LOC_URL:
            this->http_handlers.erase(source.url());
            break;
        }
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

    PackageHandler::ptr NativeProvider::get_or_add_handler(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case PackageSource::LOC_VFS:
            return this->vfs_handlers.emplace_shared(
                source.vfs_path(this->default_vfs_path),
                this->settings,
                source.vfs_path(this->default_vfs_path));

        case PackageSource::LOC_URL:
            return this->http_handlers.emplace_shared(
                source.url(this->default_url),
                this->settings,
                source.url(this->default_url));

        default:
            return {};
        }
    }

}  // namespace platform::upgrade::native
