// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-provider.c++
/// @brief Upgrade service - gRPC client implementation
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "upgrade-grpc-provider.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace upgrade::grpc
{
    ClientProvider::ClientProvider(const std::shared_ptr<Client> &client)
        : Super("grpc"),
          client(client)
    {
    }

    bool ClientProvider::is_pertinent() const
    {
        return bool(client);
    }

    PackageCatalogue ClientProvider::scan(
        const PackageSource &source)
    {
        return protobuf::decoded<PackageCatalogue>(
            this->client->call_check(
                &Client::Stub::Scan,
                protobuf::encoded<::cc::platform::upgrade::PackageSource>(source)));
    }

    PackageSources ClientProvider::list_sources() const
    {
        return protobuf::decoded<PackageSources>(
            this->client->call_check(&Client::Stub::ListSources));
    }

    PackageCatalogue ClientProvider::list_available(
        const PackageSource &source) const
    {
        return protobuf::decoded<PackageCatalogue>(
            this->client->call_check(
                &Client::Stub::ListAvailable,
                protobuf::encoded<::cc::platform::upgrade::PackageSource>(source)));
    }

    PackageInfo::ptr ClientProvider::best_available(
        const PackageSource &source) const
    {
        return protobuf::decoded_shared<PackageInfo>(
            this->client->call_check(
                &Client::Stub::BestAvailable,
                protobuf::encoded<::cc::platform::upgrade::PackageSource>(source)));
    }

    PackageInfo::ptr ClientProvider::install(
        const PackageSource &source)
    {
        ::cc::platform::upgrade::InstallRequest request;
        protobuf::encode(source, request.mutable_source());

        return protobuf::decoded_shared<PackageInfo>(
            this->client->call_check(&Client::Stub::Install, request));
    }

    void ClientProvider::finalize()
    {
        this->client->call_check(&Client::Stub::Finalize);
    }

}  // namespace upgrade::grpc
