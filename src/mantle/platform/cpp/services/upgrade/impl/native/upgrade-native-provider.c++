// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.c++
/// @brief Upgrade implementation
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-native-provider.h++"
#include "package-handler-vfs.h++"
#include "package-handler-url.h++"
#include "upgrade-settings.h++"
#include "vfs-signals.h++"
#include "sysconfig-host.h++"
#include "platform/symbols.h++"
#include "status/exceptions.h++"
#include "chrono/scheduler.h++"
#include "types/create-shared.h++"

namespace upgrade::native
{
    NativeProvider::NativeProvider()
        : Super(TYPE_NAME_BASE(This)),
          settings(core::SettingsStore::create_shared(SETTINGS_FILE)),
          default_vfs_path(settings->get(SETTING_VFS_CONTEXT, DEFAULT_VFS_CONTEXT).as_string(), {}),
          default_url(settings->get(SETTING_SCAN_URL).as_string()),
          scan_interval(settings->get(SETTING_SCAN_INTERVAL, DEFAULT_SCAN_INTERVAL).as_duration()),
          scan_retries(settings->get(SETTING_SCAN_RETRIES, DEFAULT_SCAN_RETRIES).as_uint())
    {
    }

    void NativeProvider::initialize()
    {
        Super::initialize();
        if (!this->default_url.empty())
        {
            core::scheduler.add(
                this->name(),                                // handle
                [&] {                                        // |
                    this->scan_source({this->default_url});  // |> invocation
                },                                           // |
                this->scan_interval,                         // interval
                core::Scheduler::ALIGN_LOCAL,                // align
                core::status::Level::DEBUG,                  // loglevel
                0,                                           // count
                this->scan_retries,                          // retries
                false);                                      // catchup
        }

        using namespace std::placeholders;
        vfs::signal_context.connect(
            this->name(),
            std::bind(&This::on_vfs_context, this, _1, _2, _3));

        // vfs::signal_context_in_use.connect(
        //     this->name(),
        //     std::bind(&This::on_vfs_context_in_use, this, _1, _2, _3));
    }

    void NativeProvider::deinitialize()
    {
        // vfs::signal_context_in_use.disconnect(this->name());
        vfs::signal_context.disconnect(this->name());
        core::scheduler.remove(this->name());
        Super::deinitialize();
    }

    PackageCatalogue NativeProvider::scan(const PackageSource &source)
    {
        if (source)
        {
            this->scan_source(source);
        }
        else
        {
            if (this->default_vfs_path)
            {
                this->get_or_add_index({this->default_vfs_path})->scan();
            }
            if (!this->default_url.empty())
            {
                this->get_or_add_index({this->default_url})->scan();
            }
            this->emit_best_available();
        }

        return this->list_available(source);
    }

    PackageSources NativeProvider::list_sources() const
    {
        PackageSources sources;
        sources.reserve(this->vfs_indices.size() + this->url_indices.size());
        for (const auto &[vfs_path, index] : this->vfs_indices)
        {
            sources.emplace_back(vfs_path);
        }
        for (const auto &[url, index] : this->url_indices)
        {
            sources.emplace_back(url);
        }
        return sources;
    }

    PackageCatalogue NativeProvider::list_available(const PackageSource &source) const
    {
        if (source.empty())
        {
            PackageCatalogue available;
            for (const PackageIndex::ptr &index : this->indices())
            {
                PackageCatalogue sources = index->get_available();
                available.insert(available.end(),
                                 sources.begin(),
                                 sources.end());
            }
            return available;
        }
        else if (const PackageIndex::ptr &index = this->get_index(source))
        {
            return index->get_available();
        }
        else
        {
            throw core::exception::NotFound(
                "Package source has not been scanned",
                source.to_string());
        }
    }

    PackageInfo::ptr NativeProvider::best_available(const PackageSource &source) const
    {
        PackageInfo::ptr best;
        for (const PackageInfo::ptr &candidate : this->list_available(source))
        {
            if (candidate->is_applicable() &&
                (!best || (candidate->version() > best->version())))
            {
                best = candidate;
            }
        }
        return best;
    }

    PackageInfo::ptr NativeProvider::install(const PackageSource &source)
    {
        PackageSource install_source;

        if (source)
        {
            install_source = source;
        }
        else if (PackageInfo::ptr package_info = this->best_available(source))
        {
            install_source = package_info->source();
        }
        else
        {
            throw core::exception::NotFound(
                "No package file specified and "
                "no applicable package discovered from prior scans");
        }

        if (!this->install_lock.try_lock())
        {
            throw core::exception::Unavailable("An install task is already pending");
        }

        try
        {
            if (const PackageHandler::ptr &handler = this->get_handler(install_source))
            {
                return this->installed_package_info = handler->install(install_source);
            }
            else
            {
                return {};
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
        if (this->install_lock.locked() && this->installed_package_info)
        {
            if (auto handler = this->get_handler(this->installed_package_info->source()))
            {
                logf_notice("Finalizing upgrade");
                handler->finalize(this->installed_package_info);
            }

            this->install_lock.unlock();
        }
    }

    bool NativeProvider::remove_index(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return this->vfs_indices.erase(source.vfs_path());

        case LocationType::URL:
            return this->url_indices.erase(source.url());

        default:
            return false;
        }
    }

    std::vector<PackageIndex::ptr> NativeProvider::indices() const
    {
        std::vector<PackageIndex::ptr> indices;
        indices.reserve(this->vfs_indices.size() + this->url_indices.size());

        for (const auto &[vfs_path, index] : this->vfs_indices)
        {
            indices.push_back(index);
        }

        for (const auto &[url, index] : this->url_indices)
        {
            indices.push_back(index);
        }
        return indices;
    }

    PackageIndex::ptr NativeProvider::get_index(const PackageSource &source) const
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return this->vfs_indices.get(source.vfs_path(this->default_vfs_path));

        case LocationType::URL:
            return this->url_indices.get(source.url(this->default_url));

        default:
            return {};
        }
    }

    PackageIndex::ptr NativeProvider::get_or_add_index(const PackageSource &source)
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return this->vfs_indices.emplace_shared(
                source.vfs_path(),
                this->settings,
                source.vfs_path());

        case LocationType::URL:
            return this->url_indices.emplace_shared(
                source.url(),
                source.url());

        default:
            return {};
        }
    }

    PackageHandler::ptr NativeProvider::get_handler(const PackageSource &source) const
    {
        switch (source.location_type())
        {
        case LocationType::VFS:
            return std::make_shared<VFSPackageHandler>(this->settings);

        case LocationType::URL:
            return std::make_shared<URLPackageHandler>(this->settings);

        default:
            return {};
        }
    }

    void NativeProvider::emit_best_available() const
    {
        PackageInfo::ptr best = this->best_available({});
        PackageInfo::ptr previous = signal_upgrade_available.get_cached({});

        if (!core::types::equivalent(best, previous))
        {
            signal_upgrade_available.emit(best);
        }
    }

    void NativeProvider::scan_source(const PackageSource &source)
    {
        this->get_or_add_index(source)->scan();
        this->emit_best_available();
    }

    void NativeProvider::on_vfs_context(
        core::signal::MappingAction action,
        const vfs::ContextName &name,
        const vfs::Context::ptr &context)
    {
        if ((action == core::signal::MAP_ADDITION) && context->removable)
        {
            this->scan_source({context->virtualPath()});
        }
    }

    void NativeProvider::on_vfs_context_in_use(
        core::signal::MappingAction action,
        const vfs::ContextName &name,
        const vfs::Context::ptr &context)
    {
        if ((action == core::signal::MAP_REMOVAL) &&
            (name == this->default_vfs_path.context))
        {
            this->scan_source({context->virtualPath()});
        }
    }

}  // namespace upgrade::native
