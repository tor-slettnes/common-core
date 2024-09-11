// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-provider.c++
/// @brief Upgrade service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-provider.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace platform::upgrade::grpc
{
    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client)
        : Super("grpc"),
          client(client)
    {
    }

    bool ClientProvider::is_pertinent()
    {
        return bool(client);
    }

    PackageManifests ClientProvider::scan(
        const PackageSource &source)
    {
        return protobuf::decoded<PackageManifests>(
            this->client->call_check(
                &Client::Stub::scan,
                protobuf::encoded<cc::platform::upgrade::PackageSource>(source)));
    }

    PackageSources ClientProvider::list_sources() const
    {
        return protobuf::decoded<PackageSources>(
            this->client->call_check(&Client::Stub::list_sources));
    }

    PackageManifests ClientProvider::list_available(
        const PackageSource &source) const
    {
        return protobuf::decoded<PackageManifests>(
            this->client->call_check(
                &Client::Stub::list_available,
                protobuf::encoded<cc::platform::upgrade::PackageSource>(source)));
    }

    PackageManifest::ptr ClientProvider::best_available(
        const PackageSource &source) const
    {
        return protobuf::decode_shared<PackageManifest>(
            this->client->call_check(
                &Client::Stub::best_available,
                protobuf::encoded<cc::platform::upgrade::PackageSource>(source)));
    }

    PackageManifest::ptr ClientProvider::install(
        const PackageSource &source)
    {
        cc::platform::upgrade::InstallRequest request;
        protobuf::encode(source, request.mutable_source());

        return protobuf::decode_shared<PackageManifest>(
            this->client->call_check(&Client::Stub::install, request));
    }

    void ClientProvider::finalize()
    {
        this->client->call_check(&Client::Stub::finalize);
    }

}  // namespace platform::upgrade::grpc
