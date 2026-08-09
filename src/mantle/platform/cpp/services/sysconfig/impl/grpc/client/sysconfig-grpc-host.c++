// -*- c++ -*-
//==============================================================================
/// @file sysconfig-grpc-host.c++
/// @brief SysConfig gRPC implementation - host configuration
/// @author Tor Slettnes
//==============================================================================

#include "sysconfig-grpc-host.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-sysconfig-types.h++"
#include "protobuf-inline.h++"
#include "platform/symbols.h++"

namespace cc::platform::sysconfig::grpc
{
    HostConfigProvider::HostConfigProvider(const std::shared_ptr<Client>& client)
        : Super(TYPE_NAME_FULL(This)),
          client(client)
    {
    }

    void HostConfigProvider::initialize()
    {
        Super::initialize();
        this->client->add_handler(
            platform::sysconfig::protobuf::Signal::kHostInfo,
            [&](const platform::sysconfig::protobuf::Signal& signal) {
                sysconfig::signal_hostinfo.emit(
                    cc::protobuf::decoded<HostInfo>(signal.host_info()));
            });
    }

    HostInfo HostConfigProvider::get_host_info() const
    {
        return cc::protobuf::decoded<HostInfo>(
            this->client->call_check(
                &Client::Stub::GetHostInfo));
    }

    void HostConfigProvider::set_host_name(const std::string& hostname)
    {
        this->client->call_check(
            &Client::Stub::SetHostName,
            cc::protobuf::encoded<google::protobuf::StringValue>(hostname));
    }

    void HostConfigProvider::reboot()
    {
        this->client->call_check(
            &Client::Stub::Reboot);
    }

}  // namespace cc::platform::sysconfig::grpc
