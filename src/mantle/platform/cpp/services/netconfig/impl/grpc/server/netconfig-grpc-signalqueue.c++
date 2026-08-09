// -*- c++ -*-
//==============================================================================
/// @file netconfig-grpc-signalqueue.c++
/// @brief Connect server-side signals to gRPC stream initiated by clients
/// @author Tor Slettnes
//==============================================================================

#include "netconfig-grpc-signalqueue.h++"
#include "netconfig.h++"
#include "protobuf-netconfig-types.h++"
#include "protobuf-standard-types.h++"
#include "protobuf-inline.h++"

namespace cc::platform::netconfig::grpc
{
    void SignalQueue::initialize()
    {
        this->connect<ConnectionData::ptr>(
            protobuf::Signal::kConnection,
            signal_connection,
            [](ConnectionData::ptr ref, protobuf::Signal* msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_connection());
            });

        this->connect<ActiveConnectionData::ptr>(
            protobuf::Signal::kActiveConnection,
            signal_active_connection,
            [](ActiveConnectionData::ptr ref, protobuf::Signal* msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_active_connection());
            });

        this->connect<AccessPointData::ptr>(
            protobuf::Signal::kAccesspoint,
            signal_accesspoint,
            [](AccessPointData::ptr ref, protobuf::Signal* msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_accesspoint());
            });

        this->connect<DeviceData::ptr>(
            protobuf::Signal::kDevice,
            signal_device,
            [](DeviceData::ptr ref, protobuf::Signal* msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_device());
            });

        this->connect<GlobalData::ptr>(
            protobuf::Signal::kGlobal,
            signal_globaldata,
            [](GlobalData::ptr ref, protobuf::Signal* msg) {
                cc::protobuf::encode_shared(ref, msg->mutable_global());
            });
        Super::initialize();
    }

    void SignalQueue::deinitialize()
    {
        this->disconnect(signal_connection);
        this->disconnect(signal_active_connection);
        this->disconnect(signal_accesspoint);
        this->disconnect(signal_device);
        this->disconnect(signal_globaldata);
        Super::deinitialize();
    }
}  // namespace cc::platform::netconfig::grpc
