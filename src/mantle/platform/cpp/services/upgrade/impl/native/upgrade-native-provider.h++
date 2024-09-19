// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.h++
/// @brief Upgrade implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-index-vfs.h++"
#include "package-index-url.h++"
#include "upgrade-base.h++"
#include "upgrade-signals.h++"
#include "vfs-context.h++"
#include "thread/uniquelock.h++"
#include "types/shared_ptr_map.h++"

#include <mutex>
#include <chrono>

namespace platform::upgrade::native
{
    class NativeProvider : public ProviderInterface
    {
        using This = NativeProvider;
        using Super = ProviderInterface;

    public:
        NativeProvider();

    protected:
        void initialize() override;
        void deinitialize() override;
        PackageManifests scan(const PackageSource &source) override;
        PackageSources list_sources() const override;
        PackageManifests list_available(const PackageSource &source) const override;
        PackageManifest::ptr best_available(const PackageSource &source) const override;
        PackageManifest::ptr install(const PackageSource &source) override;
        void finalize() override;
        bool remove_index(const PackageSource &source);

    protected:
        // Package indices
        std::vector<PackageIndex::ptr> indices() const;
        PackageIndex::ptr get_index(const PackageSource &source) const;
        PackageIndex::ptr get_or_add_index(const PackageSource &source);

        // Package handlers
        PackageHandler::ptr get_handler(const PackageSource &source) const;

        // Signalling
        void emit_best_available() const;

    private:
        void scan_source(const PackageSource &source);

        void on_vfs_context(
            core::signal::MappingAction action,
            const platform::vfs::ContextName &name,
            const platform::vfs::Context::ptr &context);

        void on_vfs_context_in_use(
            core::signal::MappingAction action,
            const platform::vfs::ContextName &name,
            const platform::vfs::Context::ptr &context);

    private:
        std::shared_ptr<core::SettingsStore> settings;
        vfs::Path default_vfs_path;
        std::string default_url;
        std::chrono::system_clock::duration scan_interval;
        uint scan_retries;
        core::types::shared_ptr_map<platform::vfs::Path, VFSPackageIndex> vfs_indices;
        core::types::shared_ptr_map<URL, URLPackageIndex> url_indices;
        core::types::UniqueLock install_lock;
        PackageManifest::ptr installed_manifest;
    };
}  // namespace platform::upgrade::native
