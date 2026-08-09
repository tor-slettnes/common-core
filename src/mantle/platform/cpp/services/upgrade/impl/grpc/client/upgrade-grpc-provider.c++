// -*- c++ -*-
//==============================================================================
/// @file upgrade-grpc-provider.c++
/// @brief Upgrade service - gRPC client implementation
/// @author Tor Slettnes
//==============================================================================

#include "upgrade-grpc-provider.h++"
#include "protobuf-upgrade-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::upgrade::grpc
{
    ClientProvider::ClientProvider(const std::shared_ptr<Client>& client)
        : Super("grpc"),
          client(client)
    {
    }

    bool ClientProvider::is_pertinent() const
    {
        return bool(client);
    }

    PackageCatalogue ClientProvider::scan(
        const PackageSource& source)
    {
        return cc::protobuf::decoded<PackageCatalogue>(
            this->client->call_check(
                &Client::Stub::Scan,
                cc::protobuf::encoded<platform::upgrade::protobuf::PackageSource>(source)));
    }

    PackageSources ClientProvider::list_sources() const
    {
        return cc::protobuf::decoded<PackageSources>(
            this->client->call_check(&Client::Stub::ListSources));
    }

    PackageCatalogue ClientProvider::list_available(
        const PackageSource& source) const
    {
        return cc::protobuf::decoded<PackageCatalogue>(
            this->client->call_check(
                &Client::Stub::ListAvailable,
                cc::protobuf::encoded<platform::upgrade::protobuf::PackageSource>(source)));
    }

    PackageInfo::ptr ClientProvider::best_available(
        const PackageSource& source) const
    {
        return cc::protobuf::decoded_shared<PackageInfo>(
            this->client->call_check(
                &Client::Stub::BestAvailable,
                cc::protobuf::encoded<platform::upgrade::protobuf::PackageSource>(source)));
    }

    PackageInfo::ptr ClientProvider::install(
        const PackageSource& source)
    {
        platform::upgrade::protobuf::InstallRequest request;
        cc::protobuf::encode(source, request.mutable_source());

        return cc::protobuf::decoded_shared<PackageInfo>(
            this->client->call_check(&Client::Stub::Install, request));
    }

    void ClientProvider::finalize()
    {
        this->client->call_check(&Client::Stub::Finalize);
    }

}  // namespace cc::platform::upgrade::grpc
