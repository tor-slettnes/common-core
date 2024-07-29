// -*- c++ -*-
//==============================================================================
/// @file nework-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tslettnes@picarro.com>
//==============================================================================

#include "netconfig-grpc-signalqueue.h++"
#include "netconfig.h++"
#include "protobuf-netconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace platform::netconfig::grpc
{
    void SignalQueue::initialize()
    {
        using cc::platform::netconfig::Signal;

        this->connect<ConnectionRef>(
            Signal::kConnection,
            platform::netconfig::signal_connection,
            [](ConnectionRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_connection());
            });

        this->connect<ActiveConnectionRef>(
            Signal::kActiveConnection,
            platform::netconfig::signal_active_connection,
            [](ActiveConnectionRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_active_connection());
            });

        this->connect<AccessPointRef>(
            Signal::kAccesspoint,
            platform::netconfig::signal_accesspoint,
            [](AccessPointRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_accesspoint());
            });

        this->connect<DeviceRef>(
            Signal::kDevice,
            platform::netconfig::signal_device,
            [](DeviceRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_device());
            });

        this->connect<GlobalDataRef>(
            Signal::kGlobal,
            platform::netconfig::signal_globaldata,
            [](GlobalDataRef ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_global());
            });
        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(platform::netconfig::signal_connection);
        this->disconnect(platform::netconfig::signal_active_connection);
        this->disconnect(platform::netconfig::signal_accesspoint);
        this->disconnect(platform::netconfig::signal_device);
        this->disconnect(platform::netconfig::signal_globaldata);
        Super::deinitialize();
    }
}  // namespace platform::netconfig::grpc
