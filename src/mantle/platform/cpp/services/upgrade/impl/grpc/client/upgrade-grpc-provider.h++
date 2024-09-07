// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-provider.h++
/// @brief Upgrade service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#pragma once
#include "upgrade-base.h++"
#include "upgrade-grpc-client.h++"

namespace platform::upgrade::grpc
{
    class ClientProvider : public ProviderInterface
    {
        using This = ClientProvider;
        using Super = ProviderInterface;

    public:
        ClientProvider(const std::shared_ptr<Client> &client);

        bool is_pertinent() override;

    protected:
        void scan(const PackageSource &source) override;
        PackageSources list_sources() const override;
        PackageManifests list_available(const PackageSource &source) const override;
        PackageManifest::ptr best_available(const PackageSource &source) const override;
        PackageManifest::ptr install(const PackageSource &source) override;
        void finalize() override;

    private:
        std::shared_ptr<Client> client;
    };
}  // namespace platform::upgrade::grpc
