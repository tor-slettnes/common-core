// -*- c++ -*-
//==============================================================================
/// @file upgrade-native-provider.h++
/// @brief Upgrade implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "package-handler-vfs.h++"
#include "package-handler-http.h++"
#include "upgrade-base.h++"
#include "upgrade-signals.h++"

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
        PackageInfo::Ref get_available() const override;
        PackageInfo::Ref install(const PackageSource &source) override;
        void finalize() override;

        void remove(const PackageSource &source);

    protected:
        void scan_vfs(const vfs::Path &source);
        void scan_http(const URL &source);

        void remove_vfs(const vfs::Path &source);
        void remove_http(const URL &source);

    private:
        core::SettingsStore::Ref settings;
        std::unordered_map<platform::vfs::Path, std::shared_ptr<VFSPackageHandler>> vfs_handlers;
        std::unordered_map<URL, std::shared_ptr<HTTPPackageHandler>> http_handlers;
    };
}  // namespace platform::upgrade::native
