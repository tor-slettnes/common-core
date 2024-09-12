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

    PackageManifests NativeProvider::scan(const PackageSource &source)
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

        if (PackageManifest::ptr best = this->best_available({}))
        {
            logf_info("Emitting signal_upgrade_available: %s", *best);
        }
        else
        {
            log_info("Emitting signal_upgrade_available: {}");
        }
        signal_upgrade_available.emit(this->best_available({}));

        return this->list_available(source);
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
        if (source.empty())
        {
            PackageManifests available;
            for (const PackageHandler::ptr &handler : this->handlers())
            {
                PackageManifests handler_sources = handler->get_available();
                available.insert(available.end(),
                                 handler_sources.begin(),
                                 handler_sources.end());
            }
            return available;
        }
        else if (const PackageHandler::ptr &handler = this->get_handler(source))
        {
            return handler->get_available();
        }
        else
        {
            throw core::exception::NotFound(
                "Package source has not been scanned",
                source.to_string());
        }
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

        if (source && !source.filename().empty())
        {
            install_source = source;
        }
        else if (PackageManifest::ptr manifest = this->best_available(source))
        {
            install_source = manifest->source();
        }

        logf_notice("Installing from %s", install_source);

        if (!this->install_lock.try_lock())
        {
            throw core::exception::Unavailable("An install task is already pending");
        }

        try
        {
            if (const PackageHandler::ptr &handler =
                this->get_or_add_handler(install_source.remove_filename()))
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
            logf_notice("Finalizing upgrade");
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
        case LocationType::VFS:
            this->vfs_handlers.erase(source.vfs_path());
            break;

        case LocationType::URL:
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

    PackageHandler::ptr NativeProvider::get_handler(const PackageSource &source) const
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return this->vfs_handlers.get(source.vfs_path(this->default_vfs_path));

        case LocationType::URL:
            return this->http_handlers.get(source.url(this->default_url));

        default:
            return {};
        }
    }

    PackageHandler::ptr NativeProvider::get_or_add_handler(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return this->vfs_handlers.emplace_shared(
                source.vfs_path(this->default_vfs_path),
                this->settings,
                source.vfs_path(this->default_vfs_path));

        case LocationType::URL:
            return this->http_handlers.emplace_shared(
                source.url(this->default_url),
                this->settings,
                source.url(this->default_url));

        default:
            return {};
        }
    }

}  // namespace platform::upgrade::native
