// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.h++
/// @brief Upgrade implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-handler.h++"
#include "package-handler-vfs.h++"
#include "package-handler-http.h++"
#include "upgrade-base.h++"
#include "upgrade-signals.h++"
#include "vfs-context.h++"
#include "thread/uniquelock.h++"
#include "types/shared_ptr_map.h++"

#include <mutex>

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
        void scan(const PackageSource &source) override;
        PackageSources list_sources() const override;
        PackageManifests list_available(const PackageSource &source) const override;
        PackageManifest::ptr best_available(const PackageSource &source) const override;
        PackageManifest::ptr install(const PackageSource &source) override;
        void finalize() override;
        void remove(const PackageSource &source);

    protected:
        std::vector<PackageHandler::ptr> handlers() const;
        PackageHandler::ptr get_or_add_handler(const PackageSource &source);

    private:
        std::shared_ptr<core::SettingsStore> settings;
        vfs::Path default_vfs_path;
        std::string default_url;
        core::types::shared_ptr_map<platform::vfs::Path, VFSPackageHandler> vfs_handlers;
        core::types::shared_ptr_map<URL, HTTPPackageHandler> http_handlers;
        core::types::UniqueLock install_lock;
        PackageManifest::ptr installed_manifest;
    };
}  // namespace platform::upgrade::native
