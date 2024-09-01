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

    std::vector<PackageInfo::Ref> ClientProvider::list_available() const
    {
        std::vector<PackageInfo::Ref> list;
        ::grpc::ClientContext cxt;
        ::google::protobuf::Empty request;
        cc::platform::upgrade::PackageInfo msg;
        auto reader = this->client->stub->list_available(&cxt, request);
        while (reader->Read(&msg))
        {
            PackageInfo::Ref &info = list.emplace_back();
            protobuf::decode_shared(msg, &info);
        }
        return list;
    }

    PackageInfo::Ref ClientProvider::best_available() const
    {
        return protobuf::decode_shared<PackageInfo>(
            this->client->call_check(
                &Client::Stub::best_available));
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
