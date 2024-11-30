// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes <tor@slett.net>
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

        this->connect<ConnectionData::ptr>(
            Signal::kConnection,
            platform::netconfig::signal_connection,
            [](ConnectionData::ptr ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_connection());
            });

        this->connect<ActiveConnectionData::ptr>(
            Signal::kActiveConnection,
            platform::netconfig::signal_active_connection,
            [](ActiveConnectionData::ptr ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_active_connection());
            });

        this->connect<AccessPointData::ptr>(
            Signal::kAccesspoint,
            platform::netconfig::signal_accesspoint,
            [](AccessPointData::ptr ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_accesspoint());
            });

        this->connect<DeviceData::ptr>(
            Signal::kDevice,
            platform::netconfig::signal_device,
            [](DeviceData::ptr ref, Signal *msg) {
                protobuf::encode_shared(ref, msg->mutable_device());
            });

        this->connect<GlobalData::ptr>(
            Signal::kGlobal,
            platform::netconfig::signal_globaldata,
            [](GlobalData::ptr ref, Signal *msg) {
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
