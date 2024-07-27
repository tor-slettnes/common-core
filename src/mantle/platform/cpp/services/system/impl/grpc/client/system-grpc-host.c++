// -*- c++ -*-
//==============================================================================
/// @file system-grpc-host.c++
/// @brief System service - Host Configuration gRPC client
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "system-grpc-host.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace platform::system::grpc
{
    HostConfig::HostConfig(const std::shared_ptr<Client> &client)
        : Super("HostConfigClient"),
          client(client)
    {
    }

    void HostConfig::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            cc::platform::system::Signal::kHostInfo,
            [&](const cc::platform::system::Signal &signal) {
                platform::system::signal_hostinfo.emit(signal.host_info());
            });
    }

    HostInfo HostConfig::get_host_info() const
    {
        return this->client->call_check(
            &Client::Stub::get_host_info);
    }

    void HostConfig::set_host_name(const std::string &hostname)
    {
        this->client->call_check(
            &Client::Stub::set_host_name,
            protobuf::encoded<google::protobuf::StringValue>(hostname));
    }

    void HostConfig::reboot()
    {
        this->client->call_check(
            &Client::Stub::reboot);
    }

}  // namespace platform::system::grpc
