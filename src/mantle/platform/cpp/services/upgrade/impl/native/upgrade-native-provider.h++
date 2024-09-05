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
#include "types/shared_ptr_map.h++"

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
        PackageManifests list_available() const override;
        PackageManifest::ptr best_available() const override;
        PackageManifest::ptr install(const PackageSource &source) override;
        void finalize() override;

        void remove(const PackageSource &source);

    protected:
        void scan_vfs(const vfs::Path &source);
        void scan_http(const URL &source);

        void remove_vfs(const vfs::Path &source);
        void remove_http(const URL &source);

    protected:
        std::vector<PackageHandler::Ptr> handlers() const;

    private:
        std::shared_ptr<core::SettingsStore> settings;
        vfs::Path default_vfs_path;
        std::string default_url;
        core::types::shared_ptr_map<platform::vfs::Path, VFSPackageHandler> vfs_handlers;
        core::types::shared_ptr_map<URL, HTTPPackageHandler> http_handlers;
        PackageManifest::ptr pending_install;
        // std::unordered_map<platform::vfs::Path, std::shared_ptr<VFSPackageHandler>> vfs_handlers;
        // std::unordered_map<URL, std::shared_ptr<HTTPPackageHandler>> http_handlers;
    };
}  // namespace platform::upgrade::native
