// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-host.c++
/// @brief SysConfig gRPC implementation - host configuration
/// @author Tor Slettnes <tor@slett.net>
//==============================================================================

#include "sysconfig-grpc-host.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace sysconfig::grpc
{
    HostConfigProvider::HostConfigProvider(const std::shared_ptr<Client> &client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void HostConfigProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            ::cc::platform::sysconfig::Signal::kHostInfo,
            [&](const ::cc::platform::sysconfig::Signal &signal) {
                sysconfig::signal_hostinfo.emit(
                    protobuf::decoded<HostInfo>(signal.host_info()));
            });
    }

    HostInfo HostConfigProvider::get_host_info() const
    {
        return protobuf::decoded<HostInfo>(
            this->client->call_check(
                &Client::Stub::GetHostInfo));
    }

    void HostConfigProvider::set_host_name(const std::string &hostname)
    {
        this->client->call_check(
            &Client::Stub::SetHostName,
            protobuf::encoded<google::protobuf::StringValue>(hostname));
    }

    void HostConfigProvider::reboot()
    {
        this->client->call_check(
            &Client::Stub::Reboot);
    }

}  // namespace sysconfig::grpc
