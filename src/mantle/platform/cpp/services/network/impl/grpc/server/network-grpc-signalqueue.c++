// -*- c++ -*-
//==============================================================================
/// @file nework-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "network-grpc-signalqueue.h++"
#include "network-provider.h++"
#include "protobuf-network.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace platform::network::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::network::Signal;

        this->connect<ConnectionRef>(
            Signal::kConnection,
            platform::network::signal_connection,
            [](ConnectionRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_connection());
            });

        this->connect<ActiveConnectionRef>(
            Signal::kActiveConnection,
            platform::network::signal_active_connection,
            [](ActiveConnectionRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_active_connection());
            });

        this->connect<AccessPointRef>(
            Signal::kAccesspoint,
            platform::network::signal_accesspoint,
            [](AccessPointRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_accesspoint());
            });

        this->connect<DeviceRef>(
            Signal::kDevice,
            platform::network::signal_device,
            [](DeviceRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_device());
            });

        this->connect<GlobalDataRef>(
            Signal::kGlobal,
            platform::network::signal_globaldata,
            [](GlobalDataRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_global());
            });
        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(platform::network::signal_connection);
        this->disconnect(platform::network::signal_active_connection);
        this->disconnect(platform::network::signal_accesspoint);
        this->disconnect(platform::network::signal_device);
        this->disconnect(platform::network::signal_globaldata);
        Super::deinitialize();
    }
}  // namespace platform::network::grpc
