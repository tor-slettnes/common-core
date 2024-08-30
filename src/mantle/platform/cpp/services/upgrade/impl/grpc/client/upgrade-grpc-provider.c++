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

    void ClientProvider::scan(const PackageSource &source)
    {
        this->client->call_check(
            &Client::Stub::scan,
            protobuf::encoded<cc::platform::upgrade::PackageSource>(source));
    }

    PackageInfo::Ref ClientProvider::get_available() const
    {
        return protobuf::decode_shared<PackageInfo>(
            this->client->call_check(
                &Client::Stub::get_available));
    }

    PackageInfo::Ref ClientProvider::install(const PackageSource &source)
    {
        return protobuf::decode_shared<PackageInfo>(
            this->client->call_check(
                &Client::Stub::install,
                protobuf::encoded<cc::platform::upgrade::PackageSource>(source)));
    }

    void ClientProvider::finalize()
    {
        this->client->call_check(&Client::Stub::finalize);
    }

}  // namespace platform::upgrade::grpc
